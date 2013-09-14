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
 pivacy_cred_xml_rw.h

 XML reader/writer for pivacy credentials
 *****************************************************************************/

#ifndef _PIVACY_CRED_XML_RW_H
#define _PIVACY_CRED_XML_RW_H

#include <gmpxx.h>
#include "pivacy_credential.h"
#include <vector>
#include <memory>

/**
 * Pivacy credential XML reader/writer
 */
class pivacy_credential_xml_rw
{
public:
	/**
	 * Get the one-and-only instance of the pivacy credential XML reader/writer object
	 * @return the one-and-only instance of the pivacy credential XML reader/writer object
	 */
	static pivacy_credential_xml_rw* i();
	
	/**
	 * Reads a pivacy credential
	 * @param cred_file_name the filename of the credential file
	 * @return A new pivacy credential object
	 */
	pivacy_credential* read_pivacy_credential(const std::string cred_file_name);
	
	/**
	 * Writes out a pivacy credential
	 * @param cred_file_name the filename of the credential file
	 * @param cred the credential to write to the file
	 * @return true if the credential was successfully written to the file
	 */
	bool write_pivacy_credential(const std::string cred_file_name, pivacy_credential* cred);
	
private:
	// The one-and-only instance
	static std::auto_ptr<pivacy_credential_xml_rw> _i;
};

#endif // !_PIVACY_CRED_XML_RW_H

