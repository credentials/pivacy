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

#ifndef _PIVACY_CARDEMU_EMULATOR_H
#define _PIVACY_CARDEMU_EMULATOR_H

#include "pivacy_credential.h"
#include "silvia_bytestring.h"
#include <vector>

/**
 * Card emulator
 */
 
class pivacy_cardemu_emulator
{
public:
	/**
	 * Constructor
	 */
	pivacy_cardemu_emulator();
	
	/**
	 * Destructor
	 */
	~pivacy_cardemu_emulator();
	
	/**
	 * Process an APDU
	 * @param c_apdu the C-APDU
	 * @param r_apdu the R-APDU
	 */
	void process_apdu(bytestring& c_apdu, bytestring& r_apdu);
	
private:
	/**
	 * Reset the emulation state; called upon application selection
	 */
	void reset();
	
	/**
	 * Reset the proof state
	 */
	void reset_proof();
	
	/**
	 * Process SELECT
	 * @param c_apdu the C-APDU
	 * @param r_apdu the R-APDU
	 */
	void process_select(bytestring& c_apdu, bytestring& r_apdu);
	
	/**
	 * Process VERIFY PIN
	 * @param c_apdu the C-APDU
	 * @param r_apdu the R-APDU
	 */
	void process_verify_pin(bytestring& c_apdu, bytestring& r_apdu);
	
	/**
	 * Process PROVE CREDENTIAL
	 * @param c_apdu the C-APDU
	 * @param r_apdu the R-APDU
	 */
	void process_prove_credential(bytestring& c_apdu, bytestring& r_apdu);
	
	/**
	 * Process PROVE COMMITMENT
	 * @param c_apdu the C-APDU
	 * @param r_apdu the R-APDU
	 */
	void process_prove_commitment(bytestring& c_apdu, bytestring& r_apdu);
	
	/**
	 * Process PROVE SIGNATURE
	 * @param c_apdu the C-APDU
	 * @param r_apdu the R-APDU
	 */
	void process_prove_signature(bytestring& c_apdu, bytestring& r_apdu);
	
	/**
	 * Process GET RESPONSE
	 * @param c_apdu the C-APDU
	 * @param r_apdu the R-APDU
	 */
	void process_get_response(bytestring& c_apdu, bytestring& r_apdu);

	/* The credentials */
	std::vector<pivacy_credential*> credentials;
	
	/* The selected credential*/
	pivacy_credential* selected_credential;
	
	/* The current proof */
	bool proof_started;
	bool proof_have_context_and_D;
	bool proof_proved;
	std::vector<bool> curproof_D;
	bytestring curproof_context;
	bytestring curproof_A_prime;
	bytestring curproof_e_hat;
	bytestring curproof_v_prime_hat;
	std::vector<bytestring> curproof_attributes;
	
	/* Authentication */
	bytestring user_PIN;
	bytestring admin_PIN;
	bool user_PIN_verified;
	bool admin_PIN_verified;
};

#endif // !_PIVACY_CARDEMU_EMULATOR_H

