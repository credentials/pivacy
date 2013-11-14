/*
 * Copyright (c) 2013 Roland van Rijswijk-Deij
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*****************************************************************************
 pivacy_cardemu_emulator.h

 The actual IRMA card emulation
 *****************************************************************************/

#include "config.h"
#include "pivacy_cardemu_emulator.h"
#include "pivacy_log.h"
#include "pivacy_errors.h"
#include "pivacy_config.h"
#include "pivacy_cred_xml_rw.h"
#include "silvia_parameters.h"
#include "silvia_prover.h"
#include "pivacy_ui_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Status words */
#define SW_OK								"9000"
#define SW_LENGTH_ERROR						"6700"
#define SW_SECURITY_STATUS_NOT_SATISFIED	"6982"
#define SW_WRONG_STATE						"6985"
#define SW_DATA_UNKNOWN						"6b00"
#define SW_APPLICATION_UNKNOWN				"6a82"
#define SW_PIN_INCORRECT					"63c0"
#define SW_CREDENTIAL_UNKNOWN				"6a80"
#define SW_ATTRIBUTE_UNKNOWN				"6a88"
#define SW_ALREADY_SELECTED					"6986"
#define SW_UNKNOWN_INS						"6d00"
#define SW_UNKNOWN_CLA						"6e00"
#define SW_UNKNOWN_ERROR					"6f00"

/* APDU classes */
#define CLA_ISO					0x00
#define CLA_PRIVATE				0x80

/* APDU instructions */
#define INS_SELECT				0xa4
#define INS_VERIFY_PIN			0x20

#define INS_PROVE_CREDENTIAL	0x20
#define INS_PROVE_COMMITMENT	0x2a
#define INS_PROVE_SIGNATURE		0x2b
#define INS_GET_RESPONSE		0x2c

/* APDU offsets */
#define	OFS_CLA					0
#define OFS_INS					1
#define OFS_P1					2
#define OFS_P2					3
#define OFS_LC					4
#define OFS_CDATA				5

#define DEFAULT_USER_PIN		"00000000"
#define DEFAULT_ADMIN_PIN		"000000000000"

pivacy_cardemu_emulator::pivacy_cardemu_emulator()
{
	pivacy_ui_lib_init();
	
	reset();
	
	/* Load credentials */
	std::string credential_dir;
	
	if ((pivacy_conf_get_string("emulation.credentials", "directory", credential_dir, NULL) != PRV_OK) || credential_dir.empty())
	{
		ERROR_MSG("Failed to retrieve credential directory from the configuration");
		
		return;
	}
	
	/* 
	 * Enumerate all files in the directory and tried to load all
	 * files with the extension .xml as if they were a pivacy
	 * credential
	 */
	DIR* dir = opendir(credential_dir.c_str());
	
	if (dir == NULL)
	{
		ERROR_MSG("Failed to open directory %s, cannot load credentials", credential_dir.c_str());
		
		return;
	}
	
	struct dirent* entry = NULL;
	
	while ((entry = readdir(dir)) != NULL)
	{
		// Check if this is the . or .. entry
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
		{
			continue;
		}
		
		// Convert the name of the entry to a C++ string
		std::string name(entry->d_name);
		bool is_regular_file = false;

#if defined(_DIRENT_HAVE_D_TYPE) && defined(_BSD_SOURCE)
		// Determine the type of the entry
		switch(entry->d_type)
		{
		case DT_DIR:
			// Do nothing
			break;
		case DT_REG:
			// This is a regular file
			is_regular_file = true;
			break;
		default:
			// Do nothing
			break;
		}
#else
		// The entry type has to be determined using lstat
		struct stat entry_status;

		std::string full_path = credential_dir + "/" + name;

		if (!lstat(full_path.c_str(), &entry_status))
		{
			if (S_ISDIR(entry_status.st_mode))
			{
				// Do nothing
			}
			else if (S_ISREG(entry_status.st_mode))
			{
				is_regular_file = true;
			}
			else
			{
				// Do nothing
			}
		}
#endif

		if (is_regular_file)
		{
			// Attempt to read the file as if it were a credential
			pivacy_credential* cred = pivacy_credential_xml_rw::i()->read_pivacy_credential(credential_dir + "/" + name);
			
			if (cred != NULL)
			{
				// Attempt to read the public key belonging to the credential
				if (cred->get_issuer_public_key(credential_dir) != NULL)
				{
					INFO_MSG("Successfully loaded credential %s issued by %s", cred->get_name().c_str(), cred->get_issuer().c_str());
				}
				
				credentials.push_back(cred);
			}
		}
	}

	ui_connected = false;
	
	pivacy_conf_get_bool("ui", "enable", use_ui, true);
	pivacy_conf_get_bool("ui", "optional", ui_optional, false);
	
	INFO_MSG("The Pivacy UI is %s", use_ui ? "enabled" : "disabled");
	INFO_MSG("Use of the Pivacy UI is %s", ui_optional ? "optional" : "mandatory");
	
	ui_connected = (pivacy_ui_connect() == PRV_OK);
	
	if (ui_connected)
	{
		pivacy_ui_show_status(PIVACY_STATE_WAIT);
	}
}
	
pivacy_cardemu_emulator::~pivacy_cardemu_emulator()
{
	for (std::vector<pivacy_credential*>::iterator i = credentials.begin(); i != credentials.end(); i++)
	{
		delete *i;
	}
	
	pivacy_ui_lib_uninit();
}

void pivacy_cardemu_emulator::process_apdu(bytestring& c_apdu, bytestring& r_apdu)
{
	if (!ui_connected)
	{
		if ((pivacy_ui_connect() != PRV_OK) && !ui_optional)
		{
			ERROR_MSG("Not connected to the UI");
			
			r_apdu = SW_UNKNOWN_ERROR;
			
			return;
		}
		
		INFO_MSG("Connected to the Pivacy UI");
		
		ui_connected = true;
	}
	
	if (c_apdu.size() < 4)
	{
		ERROR_MSG("Malformed command APDU of %d bytes", c_apdu.size());
		
		r_apdu = SW_UNKNOWN_ERROR;
		
		return;
	}
	
	switch(c_apdu[OFS_CLA])
	{
	case CLA_ISO:
		switch(c_apdu[OFS_INS])
		{
		case INS_SELECT:
			process_select(c_apdu, r_apdu);
			break;
		case INS_VERIFY_PIN:
			process_verify_pin(c_apdu, r_apdu);
			break;
		default:
			r_apdu = SW_UNKNOWN_INS;
			break;
		}
		break;
	case CLA_PRIVATE:
		switch(c_apdu[OFS_INS])
		{
		case INS_PROVE_CREDENTIAL:
			process_prove_credential(c_apdu, r_apdu);
			break;
		case INS_PROVE_COMMITMENT:
			process_prove_commitment(c_apdu, r_apdu);
			break;
		case INS_PROVE_SIGNATURE:
			process_prove_signature(c_apdu, r_apdu);
			break;
		case INS_GET_RESPONSE:
			process_get_response(c_apdu, r_apdu);
			break;
		default:
			r_apdu = SW_UNKNOWN_INS;
			break;
		}
		break;
	default:
		r_apdu = SW_UNKNOWN_CLA;
	}
}

void pivacy_cardemu_emulator::reset()
{
	user_PIN_verified = false;
	admin_PIN_verified = false;
	
	reset_proof();
}

void pivacy_cardemu_emulator::reset_proof()
{
	proof_started = false;
	proof_have_context_and_D = false;
	proof_proved = false;
	
	curproof_D.clear();
	curproof_context.wipe();
	curproof_A_prime.wipe();
	curproof_e_hat.wipe();
	curproof_v_prime_hat.wipe();
	curproof_attributes.clear();
}

void pivacy_cardemu_emulator::process_select(bytestring& c_apdu, bytestring& r_apdu)
{
	const bytestring IRMA_AID = "F849524D4163617264";
	
	if (c_apdu.size() < 5)
	{
		r_apdu = SW_LENGTH_ERROR;
	}
	else if (c_apdu.substr(OFS_CDATA, c_apdu[OFS_LC]) != IRMA_AID)
	{
		r_apdu = SW_APPLICATION_UNKNOWN;
	}
	else
	{
		// Version 0.8 + emu
		// 
		// Encoded as ISO7816 FCI record
		//
		// 0x6F YZ: FCI template (length: 0xYZ bytes)
		//   0xA5 YZ: Proprietary information encoded in BER-TLV (length: 0xYZ bytes)
		//     0x10 YZ: Sequence, version information (length: 0xYZ bytes)
		//       0x02 01: Integer, major (length: 0x01 byte)
		//       0x02 01: Integer, minor (length: 0x01 byte)
		//       0x02 01: Integer, maintenance (optional, length: 0x01 byte)
		//       0x02 01: Integer, build (optional, length: 0x01 byte)
		//       0x10 YZ: Sequence, extra information (optional, length: 0xYZ bytes)
		//         0x0C YZ: UTF-8 string, identifier (length: 0xYZ bytes)
		//         0x02 01: Integer, counter (optional, length: 0x01 byte)
		//         0x04 YZ: Octet string, data (optional, length: 0xYZ bytes)

		r_apdu = "6F11";
		  r_apdu += "A50F";
		    r_apdu += "100D";
		      r_apdu += "020100";		// major version = 0
		      r_apdu += "020108";		// minor version = 8
		      r_apdu += "1007";
		        r_apdu += "0C03454D55";	// EMU
		r_apdu += "9000";
	}
}

void pivacy_cardemu_emulator::process_verify_pin(bytestring& c_apdu, bytestring& r_apdu)
{
	if (c_apdu.size() < 5)
	{
		r_apdu = SW_LENGTH_ERROR;
	}
	else if ((c_apdu[OFS_P1] != 0x00) || ((c_apdu[OFS_P2] != 0x00) && (c_apdu[OFS_P2] != 0x01)))
	{
		ERROR_MSG("Invalid VERIFY PIN request");
		
		r_apdu = SW_DATA_UNKNOWN;
	}
	else
	{
		bytestring supplied_PIN = c_apdu.substr(OFS_CDATA, c_apdu[OFS_LC]);
		
		if (c_apdu[OFS_P2] == 0x00)
		{
			if (supplied_PIN == user_PIN)
			{
				r_apdu = SW_OK;
				
				user_PIN_verified = true;
			}
			else
			{
				r_apdu = SW_PIN_INCORRECT;
				
				user_PIN_verified = false;
			}
		}
		else if (c_apdu[OFS_P2] == 0x01)
		{
			if (supplied_PIN == admin_PIN)
			{
				r_apdu = SW_OK;
				
				admin_PIN_verified = true;
			}
			else
			{
				r_apdu = SW_PIN_INCORRECT;
				
				admin_PIN_verified = false;
			}
		}
	}
}

void pivacy_cardemu_emulator::process_prove_credential(bytestring& c_apdu, bytestring& r_apdu)
{
	reset_proof();
	
	if ((c_apdu.size() < 5) || (c_apdu[OFS_LC] != 0x28))
	{
		r_apdu = SW_LENGTH_ERROR;
	}
	else if ((c_apdu[OFS_P1] != 0x00) || (c_apdu[OFS_P2] != 0x00))
	{
		r_apdu = SW_WRONG_STATE;
	}
	else
	{
		unsigned short credential_id = (c_apdu[OFS_CDATA] >> 8) + c_apdu[OFS_CDATA + 1];
		
		/* Check if this credential exists */
		selected_credential = NULL;
		
		for (std::vector<pivacy_credential*>::iterator i = credentials.begin(); i != credentials.end(); i++)
		{
			if ((*i)->get_credential_id() == credential_id)
			{
				selected_credential = *i;
			}
		}
		
		if (selected_credential == NULL)
		{
			ERROR_MSG("Attempt to start proof for non-existent credential 0x%04X", credential_id);
			
			r_apdu = SW_CREDENTIAL_UNKNOWN;
		}
		else
		{
			unsigned short D_val = (c_apdu[OFS_CDATA + 2] << 8) + c_apdu[OFS_CDATA + 2 + 1];
			
			curproof_context = c_apdu.substr(OFS_CDATA + 2 + 2, SYSPAR(l_H) / 8);
			
			time_t timestamp = (c_apdu[OFS_CDATA + 2 + (SYSPAR(l_H) / 8) + 2] << 24) +
			                   (c_apdu[OFS_CDATA + 2 + (SYSPAR(l_H) / 8) + 3] << 16) +
			                   (c_apdu[OFS_CDATA + 2 + (SYSPAR(l_H) / 8) + 4] << 8) +
			                   (c_apdu[OFS_CDATA + 2 + (SYSPAR(l_H) / 8) + 5]);
			                   
			INFO_MSG("Started proof with context = %s, D = 0x%04X, timestamp = %d", curproof_context.hex_str().c_str(), D_val, timestamp);
			
			/* Convert D to vector of booleans, start at "expiry" */
			unsigned short D_mask = 0x0002;
			std::vector<const char*> display_attributes;
			
			for (int i = 0; i < selected_credential->get_silvia_credential()->num_attributes(); i++)
			{
				if (FLAG_SET(D_val, D_mask))
				{
					curproof_D.push_back(true);
					
					INFO_MSG("Revealing attribute %s", selected_credential->get_attribute_names()[i].c_str());
					
					display_attributes.push_back(selected_credential->get_attribute_names()[i].c_str());
				}
				else
				{
					curproof_D.push_back(false);
					
					INFO_MSG("Keeping attribute %s hidden", selected_credential->get_attribute_names()[i].c_str());
				}
				
				D_mask <<= 1;
			}
			
			r_apdu = SW_OK;
				
			proof_started = true;
			proof_have_context_and_D = true;
			
			if (ui_connected)
			{
				int consent_result;
				pivacy_rv rv;
				
				if (((rv = pivacy_ui_consent("This terminal", &display_attributes[0], display_attributes.size(), 0, &consent_result) != PRV_OK) ||
				    ((rv = pivacy_ui_show_status(PIVACY_STATE_PRESENT)) != PRV_OK)) && !ui_optional)
				{
					reset_proof();
					
					r_apdu = SW_UNKNOWN_ERROR;
				}
				
				if (rv == PRV_OK)
				{
					if (consent_result == PIVACY_CONSENT_NO)
					{
						reset_proof();
						
						r_apdu = SW_SECURITY_STATUS_NOT_SATISFIED;
					}
				}
			}
		}
	}
}

void pivacy_cardemu_emulator::process_prove_commitment(bytestring& c_apdu, bytestring& r_apdu)
{
	if (!proof_started || !proof_have_context_and_D || (selected_credential == NULL))
	{
		r_apdu = SW_WRONG_STATE;
		reset_proof();
	}
	else if ((c_apdu.size() < 5) || (c_apdu[OFS_LC] != (SYSPAR(l_statzk) / 8)))
	{
		r_apdu = SW_LENGTH_ERROR;
		reset_proof();
	}
	else if ((c_apdu[OFS_P1] != 0x00) || (c_apdu[OFS_P2] != 0x00))
	{
		r_apdu = SW_DATA_UNKNOWN;
		reset_proof();
	}
	else
	{
		// Retrieve the nonce
		bytestring nonce = c_apdu.substr(OFS_CDATA, c_apdu[OFS_LC]);
		
		// Generate the proof
		silvia_prover prover(selected_credential->get_issuer_public_key(), selected_credential->get_silvia_credential());
		
		mpz_class c;
		mpz_class A_prime;
		mpz_class e_hat;
		mpz_class v_prime_hat;
		std::vector<mpz_class> a_i_hat;
		std::vector<silvia_attribute*> a_i;
		
		prover.prove(curproof_D, nonce.mpz_val(), curproof_context.mpz_val(), c, A_prime, e_hat, v_prime_hat, a_i_hat, a_i);
		
		// Save proof output
		std::vector<mpz_class>::iterator a_i_hat_it = a_i_hat.begin();
		std::vector<silvia_attribute*>::iterator a_i_it = a_i.begin();
		
		curproof_A_prime = bytestring(A_prime);
		curproof_e_hat = bytestring(e_hat);
		curproof_v_prime_hat = bytestring(v_prime_hat);
		
		/* Add hidden master secret */
		curproof_attributes.push_back(bytestring(*a_i_hat_it));
		a_i_hat_it++;
		
		for (std::vector<bool>::iterator i = curproof_D.begin(); i != curproof_D.end(); i++)
		{
			if ((*i) == true)
			{
				curproof_attributes.push_back(bytestring((*a_i_it)->rep()));
				a_i_it++;
			}
			else
			{
				curproof_attributes.push_back(bytestring(*a_i_hat_it));
				a_i_hat_it++;
			}
		}
		
		// Return c
		r_apdu = bytestring(c);
		r_apdu += SW_OK;
		
		proof_proved = true;
	}
}

void pivacy_cardemu_emulator::process_prove_signature(bytestring& c_apdu, bytestring& r_apdu)
{
	if (c_apdu[OFS_P2] != 0x00)
	{
		r_apdu = SW_DATA_UNKNOWN;
		reset_proof();
	}
	else if (!proof_proved)
	{
		r_apdu = SW_WRONG_STATE;
		reset_proof();
	}
	else
	{
		switch(c_apdu[OFS_P1])
		{
		case 0x01: // A'
			r_apdu = curproof_A_prime;
			r_apdu += SW_OK;
			break;
		case 0x02: // e^
			r_apdu = curproof_e_hat;
			r_apdu += SW_OK;
			break;
		case 0x03: // v'^
			r_apdu = curproof_v_prime_hat;
			r_apdu += SW_OK;
			break;
		default:   // ????
			r_apdu = SW_DATA_UNKNOWN;
			reset_proof();
			break;
		}
	}
}

void pivacy_cardemu_emulator::process_get_response(bytestring& c_apdu, bytestring& r_apdu)
{
	if (c_apdu[OFS_P2] != 0x00)
	{
		r_apdu = SW_DATA_UNKNOWN;
		
		reset_proof();
	}
	else if (!proof_proved)
	{
		r_apdu = SW_WRONG_STATE;
		
		reset_proof();
	}
	else if (c_apdu[OFS_P1] > curproof_attributes.size() + 1)
	{
		r_apdu = SW_DATA_UNKNOWN;
		
		reset_proof();
	}
	else
	{
		r_apdu = curproof_attributes[c_apdu[OFS_P1]];
		r_apdu += "9000";
		
		if (c_apdu[OFS_P1] == (curproof_attributes.size() - 1))
		{
			reset_proof();
		}
	}
}

void pivacy_cardemu_emulator::power_up()
{
	DEBUG_MSG("POWER UP received, resetting");
	
	reset();
	
	if (ui_connected)
	{
		pivacy_ui_show_status(PIVACY_STATE_PRESENT);
	}
}

void pivacy_cardemu_emulator::power_down()
{
	DEBUG_MSG("POWER DOWN received, resetting");
	
	reset();
	
	if (ui_connected)
	{
		pivacy_ui_show_status(PIVACY_STATE_WAIT);
	}
}
