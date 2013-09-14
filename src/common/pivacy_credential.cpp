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
 pivacy_credential.cpp

 Pivacy credential object (encapsulates a silvia credential object)
 *****************************************************************************/

#include "config.h"
#include "pivacy_credential.h"

pivacy_credential::pivacy_credential(const std::string& name, const std::string& issuer)
{
	silvia_cred = NULL;
	cred_id = 0;
	this->name = name;
	this->issuer = issuer;
}

pivacy_credential::~pivacy_credential()
{
	if (silvia_cred != NULL)
	{
		delete silvia_cred;
	}
}

const std::string& pivacy_credential::get_name()
{
	return name;
}

const std::string& pivacy_credential::get_issuer()
{
	return issuer;
}

void pivacy_credential::set_silvia_credential(silvia_credential* silvia_cred)
{
	if (this->silvia_cred != NULL)
	{
		delete this->silvia_cred;
	}
	
	this->silvia_cred = silvia_cred;
}

silvia_credential* pivacy_credential::get_silvia_credential()
{
	return silvia_cred;
}

void pivacy_credential::set_credential_id(unsigned short cred_id)
{
	this->cred_id = cred_id;
}

unsigned short pivacy_credential::get_credential_id()
{
	return cred_id;
}

void pivacy_credential::add_attribute_name(const std::string attr_name)
{
	attribute_names.push_back(attr_name);
}

const std::vector<std::string>& pivacy_credential::get_attribute_names()
{
	return attribute_names;
}
