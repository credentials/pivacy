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
 pivacy_credgen.cpp

 Credential generator for the Pivacy; issues new credentials based on the
 specifications on the command line
 *****************************************************************************/

#include "config.h"
#include "silvia_types.h"
#include "silvia_parameters.h"
#include "silvia_idemix_xmlreader.h"
#include "silvia_issuer.h"
#include "silvia_prover_credgen.h"
#include "pivacy_cred_xml_rw.h"
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

const char* weekday[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

const char* month[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

void set_parameters()
{
	////////////////////////////////////////////////////////////////////
	// Set the system parameters in the IRMA library; this function must
	// be updated if we ever change the parameters for IRMA cards!!!
	////////////////////////////////////////////////////////////////////
	
	silvia_system_parameters::i()->set_l_n(1024);
	silvia_system_parameters::i()->set_l_m(256);
	silvia_system_parameters::i()->set_l_statzk(80);
	silvia_system_parameters::i()->set_l_H(256);
	silvia_system_parameters::i()->set_l_v(1700);
	silvia_system_parameters::i()->set_l_e(597);
	silvia_system_parameters::i()->set_l_e_prime(120);
	silvia_system_parameters::i()->set_hash_type("sha256");
}

void version(void)
{
	printf("Pivacy credential generator version %s\n", VERSION);
	printf("\n");
	printf("Copyright (c) 2013 Roland van Rijswijk-Deij\n\n");
	printf("Use, modification and redistribution of this software is subject to the terms\n");
	printf("of the license agreement. This software is licensed under a 2-clause BSD-style\n");
	printf("license a copy of which is included as the file LICENSE in the distribution.\n");
}

void usage(void)
{
	printf("Pivacy credential generator version %s\n\n", VERSION);
	printf("Usage:\n");
	printf("\tpivacy_credgen -c <cred-spec> -o <cred-file> -p <issuer-pubkey> -s <issuer-privkey>");
	printf("\n");
	printf("\tpivacy_credgen -h\n");
	printf("\tpivacy_credgen -v\n");
	printf("\n");
	printf("\t-c <cred-spec>      Read credential specification from <cred-spec>\n");
	printf("\t-o <cred-file>      Write issued credential to <cred-file>\n");
	printf("\t-p <issuer-pubkey>  Read issuer public key from <issuer-pubkey>\n");
	printf("\t-s <issuer-privkey> Read issuer private key from <issuer-privkey>\n");
	printf("\n");
	printf("\t-h                  Print this help message\n");
	printf("\n");
	printf("\t-v                  Print the version number\n");
}

int main(int argc, char* argv[])
{
	// Set library parameters
	set_parameters();
	
	// Program parameters
	std::string cred_spec;
	std::string cred_file;
	std::string issuer_pubkey;
	std::string issuer_privkey;
	int c = 0;
	
	while ((c = getopt(argc, argv, "c:o:p:s:hv")) != -1)
	{
		switch (c)
		{
		case 'h':
			usage();
			return 0;
		case 'v':
			version();
			return 0;
		case 'c':
			cred_spec = std::string(optarg);
			break;
		case 'o':
			cred_file = std::string(optarg);
			break;
		case 'p':
			issuer_pubkey = std::string(optarg);
			break;
		case 's':
			issuer_privkey = std::string(optarg);
			break;
		}
	}
	
	if (cred_spec.empty())
	{
		fprintf(stderr, "No credential specification file specified on the command line!\n");
		
		return -1;
	}
	
	if (cred_file.empty())
	{
		fprintf(stderr, "No credential specification file specified on the command line!\n");
		
		return -1;
	}
	
	if (issuer_pubkey.empty())
	{
		fprintf(stderr, "No issuer public key file specified on the command line!\n");
		
		return -1;
	}
	
	if (issuer_privkey.empty())
	{
		fprintf(stderr, "No issuer private key file specified on the command line!\n");
		
		return -1;
	}
	
	// Read issuer public and private key
	silvia_pub_key* issuer_public_key = silvia_idemix_xmlreader::i()->read_idemix_pubkey(issuer_pubkey);
	
	if (issuer_public_key == NULL)
	{
		fprintf(stderr, "Failed to read issuer public key from %s\n", issuer_pubkey.c_str());
		
		return -1;
	}
	
	silvia_priv_key* issuer_private_key = silvia_idemix_xmlreader::i()->read_idemix_privkey(issuer_privkey);
	
	if (issuer_private_key == NULL)
	{
		fprintf(stderr, "Failed to read issuer private key from %s\n", issuer_privkey.c_str());
		
		delete issuer_public_key;
		
		return -1;
	}
	
	printf("Read issuer public and private key.\n");
	
	// Set up a new issuer object
	silvia_issuer issuer(issuer_public_key, issuer_private_key);
	
	// Read the credential specification
	pivacy_credential* pivacy_cred = pivacy_credential_xml_rw::i()->read_pivacy_credential(cred_spec);
	
	if (pivacy_cred == NULL)
	{
		fprintf(stderr, "Failed to read credential specification from %s\n", cred_spec.c_str());
		
		delete issuer_public_key;
		delete issuer_private_key;
		
		return -1;
	}
	
	printf("Read credential specification.\n");
	
	// Issue new credential
	printf("Issuing new credential... "); fflush(stdout);
	
	silvia_credential_generator credgen(issuer_public_key);
	
	credgen.new_secret();
	credgen.set_attributes(pivacy_cred->get_silvia_credential()->get_attributes());
	issuer.set_attributes(pivacy_cred->get_silvia_credential()->get_attributes());
	
	mpz_class U;
	mpz_class v_prime;
	
	credgen.compute_commitment(U, v_prime);
	
	mpz_class issuer_nonce = issuer.get_issuer_nonce();
	
	mpz_class context = 0;
	
	mpz_class c_val;
	mpz_class v_prime_hat;
	mpz_class s_hat;
	
	credgen.prove_commitment(issuer_nonce, context, c_val, v_prime_hat, s_hat);
	
	if (!issuer.submit_and_verify_commitment(context, U, c_val, v_prime_hat, s_hat))
	{
		printf("FAILED\n");
		
		delete pivacy_cred;
		delete issuer_public_key;
		delete issuer_private_key;
	}
	
	mpz_class A;
	mpz_class e;
	mpz_class v_prime_prime;
	
	issuer.compute_signature(A, e, v_prime_prime);
	
	mpz_class prover_nonce = credgen.get_prover_nonce();
	
	mpz_class e_hat;
	
	issuer.prove_signature(prover_nonce, context, c_val, e_hat);
	
	if (!credgen.verify_signature(context, A, e, c_val, e_hat))
	{
		printf("FAILED\n");
		
		delete pivacy_cred;
		delete issuer_public_key;
		delete issuer_private_key;
	}
	
	credgen.compute_credential(A, e, v_prime_prime);
	
	if (!credgen.verify_credential())
	{
		printf("FAILED\n");
		
		delete pivacy_cred;
		delete issuer_public_key;
		delete issuer_private_key;
	}
	
	pivacy_cred->set_silvia_credential(credgen.get_credential());
	
	printf("OK\n");
	
	// Write out the credential
	if (!pivacy_credential_xml_rw::i()->write_pivacy_credential(cred_file, pivacy_cred))
	{
		fprintf(stderr, "Failed to write credential to %s\n", cred_file.c_str());
	}
	else
	{
		printf("Successfully wrote newly issued credential to %s\n", cred_file.c_str());
	}
	
	// Clean up
	std::vector<silvia_attribute*>::const_iterator attr_it = pivacy_cred->get_silvia_credential()->get_attributes().begin();
	
	while (attr_it != pivacy_cred->get_silvia_credential()->get_attributes().end())
	{
		delete *attr_it;
		
		attr_it++;
	}
	
	delete pivacy_cred;
	delete issuer_public_key;
	delete issuer_private_key;
	
	return 0;
}
