/* $Id$ */

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
 pivacy_credential.h

 Pivacy credential object (encapsulates a silvia credential object)
 *****************************************************************************/

#ifndef _PIVACY_CREDENTIAL_H
#define _PIVACY_CREDENTIAL_H

#include <gmpxx.h>
#include "silvia_types.h"
#include <vector>
#include <string>

class pivacy_credential
{
public:
	/**
	 * Constructor
	 * @param name the name of the credential
	 * @param issuer the issuer of the credential
	 */
	pivacy_credential(const std::string& name, const std::string& issuer);
	
	/**
	 * Destructor
	 */
	~pivacy_credential();
	
	/**
	 * Retrieve the name of the credential
	 * @return the name of the credential
	 */
	const std::string& get_name();
	
	/**
	 * Retrieve the issuer of the credential
	 * @return the issuer of the credential
	 */
	const std::string& get_issuer();
	
	/**
	 * Set the encapsulated silvia credential. Note: the pivacy
	 * credential takes ownership of the silvia credential and will
	 * delete it upon destruction
	 * @param silvia_cred the silvia credential
	 */
	void set_silvia_credential(silvia_credential* silvia_cred);
	
	/**
	 * Retrieve the encapsulated silvia credential
	 * @return the encapsulated silvia credential, or NULL if none was set
	 */
	silvia_credential* get_silvia_credential();
	
	/**
	 * Set the credential ID
	 * @param cred_id the credential ID
	 */
	void set_credential_id(unsigned short cred_id);
	
	/**
	 * Get the credential ID
	 * @return the credential ID
	 */
	unsigned short get_credential_id();
	
	/**
	 * Add an attribute name; attribute names should be added in the
	 * order they are in the encapsulated silvia credential
	 */
	void add_attribute_name(const std::string attr_name);
	
	/**
	 * Get the attribute names
	 * @return a vector containing the attribute names
	 */
	const std::vector<std::string>& get_attribute_names();
	
private:
	std::string name;
	std::string issuer;

	std::vector<std::string> attribute_names;
	
	silvia_credential* silvia_cred;
	
	unsigned short cred_id;
};

#endif // !_PIVACY_CREDENTIAL_H

