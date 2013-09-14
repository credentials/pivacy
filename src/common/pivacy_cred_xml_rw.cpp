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
 pivacy_cred_xml_rw.cpp

 XML reader/writer for pivacy credentials
 *****************************************************************************/

#include "config.h"
#include <gmpxx.h>
#include <vector>
#include <memory>
#include <assert.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "pivacy_cred_xml_rw.h"
#include "silvia_bytestring.h"

// Initialise the one-and-only instance
/*static*/ std::auto_ptr<pivacy_credential_xml_rw> pivacy_credential_xml_rw::_i(NULL);

/*static*/ pivacy_credential_xml_rw* pivacy_credential_xml_rw::i()
{
	if (_i.get() == NULL)
	{
		_i = std::auto_ptr<pivacy_credential_xml_rw>(new pivacy_credential_xml_rw());
	}

	return _i.get();
}

pivacy_credential* pivacy_credential_xml_rw::read_pivacy_credential(const std::string cred_file_name)
{
	////////////////////////////////////////////////////////////////////
	// Read the credential XML file
	////////////////////////////////////////////////////////////////////
	xmlDocPtr xmldoc = xmlParseFile(cred_file_name.c_str());
	
	// Check integrity
	xmlNodePtr root_elem = xmlDocGetRootElement(xmldoc);
	
	if ((root_elem == NULL) || (xmlStrcasecmp(root_elem->name, (const xmlChar*) "Credential") != 0))
	{
		xmlFreeDoc(xmldoc);
		
		return NULL;
	}
	
	// Parse the data
	std::string name;
	bool name_set = false;
	std::string issuer;
	bool issuer_set = false;
	unsigned short id = 0;
	bool id_set = false;
	silvia_integer_attribute secret(0);
	std::vector<std::string> attribute_names;
	std::vector<silvia_attribute*> attribute_values;
	mpz_class A(0);
	mpz_class e(0);
	mpz_class v(0);
	
	xmlNodePtr child_elem = root_elem->xmlChildrenNode;
	
	while (child_elem != NULL)
	{
		if (xmlStrcasecmp(child_elem->name, (const xmlChar*) "Name") == 0)
		{
			xmlChar* name_value = xmlNodeListGetString(xmldoc, child_elem->xmlChildrenNode, 1);
			
			if (name_value != NULL)
			{
				name = std::string((const char*) name_value);
				
				xmlFree(name_value);
				
				name_set = true;
			}
		}
		else if (xmlStrcasecmp(child_elem->name, (const xmlChar*) "IssuerID") == 0)
		{
			xmlChar* issuer_value = xmlNodeListGetString(xmldoc, child_elem->xmlChildrenNode, 1);
			
			if (issuer_value != NULL)
			{
				issuer = std::string((const char*) issuer_value);
				
				xmlFree(issuer_value);
				
				issuer_set = true;
			}
		}
		else if (xmlStrcasecmp(child_elem->name, (const xmlChar*) "Id") == 0)
		{
			xmlChar* id_value = xmlNodeListGetString(xmldoc, child_elem->xmlChildrenNode, 1);
			
			if (id_value != NULL)
			{
				id = (unsigned short) atoi((const char*) id_value);
				
				xmlFree(id_value);
				
				id_set = true;
			}
		}
		else if (xmlStrcasecmp(child_elem->name, (const xmlChar*) "Secret") == 0)
		{
			xmlChar* secret_value = xmlNodeListGetString(xmldoc, child_elem->xmlChildrenNode, 1);
			
			if (secret_value != NULL)
			{
				secret = mpz_class((const char*) secret_value);
				
				xmlFree(secret_value);
			}
		}
		else if (xmlStrcasecmp(child_elem->name, (const xmlChar*) "Attributes") == 0)
		{
			xmlNodePtr attribute = child_elem->xmlChildrenNode;
			
			while (attribute != NULL)
			{
				if (xmlStrcasecmp(attribute->name, (const xmlChar*) "Attribute") == 0)
				{
					// Get attribute type
					silvia_attr_t silvia_attr_type = SILVIA_UNDEFINED_ATTR;
					
					xmlChar* attr_type = xmlGetProp(attribute, (const xmlChar*) "type");
					
					if (attr_type == NULL)
					{
						// Malformed specification!
						xmlFreeDoc(xmldoc);
						
						return NULL;
					}
					
					if (xmlStrcasecmp(attr_type, (const xmlChar*) "int") == 0)
					{
						silvia_attr_type = SILVIA_INT_ATTR;
					}
					else if (xmlStrcasecmp(attr_type, (const xmlChar*) "string") == 0)
					{
						silvia_attr_type = SILVIA_STRING_ATTR;
					}
					
					xmlFree(attr_type);
					
					// Now read the value and the name
					std::string name;
					std::string value;
					
					xmlNodePtr attribute_data = attribute->xmlChildrenNode;
					
					while (attribute_data != NULL)
					{
						if (xmlStrcasecmp(attribute_data->name, (const xmlChar*) "Name") == 0)
						{
							xmlChar* attribute_name = xmlNodeListGetString(xmldoc, attribute_data->xmlChildrenNode, 1);
							
							if (attribute_name != NULL)
							{
								name = std::string((const char*) attribute_name);
								
								xmlFree(attribute_name);
							}
						}
						else if (xmlStrcasecmp(attribute_data->name, (const xmlChar*) "Value") == 0)
						{
							xmlChar* attribute_value = xmlNodeListGetString(xmldoc, attribute_data->xmlChildrenNode, 1);
							
							if (attribute_value != NULL)
							{
								value = std::string((const char*) attribute_value);
								
								xmlFree(attribute_value);
							}
						}
						
						attribute_data = attribute_data->next;
					}
					
					if (name.empty() || value.empty())
					{
						// Malformed specification
						xmlFreeDoc(xmldoc);
						
						return NULL;
					}
					
					attribute_names.push_back(name);
					
					switch(silvia_attr_type)
					{
					case SILVIA_INT_ATTR:
						{
							silvia_integer_attribute* new_attr = new silvia_integer_attribute(mpz_class(value.c_str()));
							attribute_values.push_back(new_attr);
						}
						break;
					case SILVIA_STRING_ATTR:
						{
							// FIXME: there is no check to see if the integer representation overflows the system parameter value l_m
							bytestring int_val((const unsigned char*) value.c_str(), value.size());
						
							silvia_integer_attribute* new_attr = new silvia_integer_attribute(int_val.mpz_val());
							attribute_values.push_back(new_attr);
						}
						break;
					default:
						// Malformed specification
						xmlFreeDoc(xmldoc);
						
						return NULL;
					}
				}
				
				attribute = attribute->next;
			}
		}
		else if (xmlStrcasecmp(child_elem->name, (const xmlChar*) "Signature") == 0)
		{
			bool A_set = false;
			bool e_set = false;
			bool v_set = false;
			
			xmlNodePtr sig_component = child_elem->xmlChildrenNode;
			
			while (sig_component != NULL)
			{
				if (xmlStrcasecmp(sig_component->name, (const xmlChar*) "A") == 0)
				{
					xmlChar* A_val = xmlNodeListGetString(xmldoc, sig_component->xmlChildrenNode, 1);
					
					if (A_val != NULL)
					{
						A = mpz_class((const char*) A_val);
						
						A_set = true;
					}
				}
				if (xmlStrcasecmp(sig_component->name, (const xmlChar*) "e") == 0)
				{
					xmlChar* e_val = xmlNodeListGetString(xmldoc, sig_component->xmlChildrenNode, 1);
					
					if (e_val != NULL)
					{
						e = mpz_class((const char*) e_val);
						
						e_set = true;
					}
				}
				if (xmlStrcasecmp(sig_component->name, (const xmlChar*) "v") == 0)
				{
					xmlChar* v_val = xmlNodeListGetString(xmldoc, sig_component->xmlChildrenNode, 1);
					
					if (v_val != NULL)
					{
						v = mpz_class((const char*) v_val);
						
						v_set = true;
					}
				}
				
				sig_component = sig_component->next;
			}
			
			if (!A_set || !e_set || !v_set)
			{
				// Malformed specification
				xmlFreeDoc(xmldoc);
				
				return NULL;
			}
		}
		
		child_elem = child_elem->next;
	}
	
	xmlFreeDoc(xmldoc);
	
	if (!name_set || !issuer_set || !id_set || attribute_names.empty() || attribute_values.empty() || (attribute_names.size() != attribute_values.size()))
	{
		return NULL;
	}
	
	// Construct credential
	pivacy_credential* pivacy_cred = new pivacy_credential(name, issuer);
	
	// Add attribute names
	for (std::vector<std::string>::iterator i = attribute_names.begin(); i != attribute_names.end(); i++)
	{
		pivacy_cred->add_attribute_name(*i);
	}
	
	// Set ID
	pivacy_cred->set_credential_id(id);
	
	// Construct internal silvia credential
	silvia_credential* silvia_cred = new silvia_credential(secret, attribute_values, A, e, v);
	
	pivacy_cred->set_silvia_credential(silvia_cred);
	
	return pivacy_cred;
}
	
bool pivacy_credential_xml_rw::write_pivacy_credential(const std::string cred_file_name, pivacy_credential* cred)
{
	FILE* cred_file = fopen(cred_file_name.c_str(), "w");
	
	if (cred_file == NULL)
	{
		return false;
	}
	
	fprintf(cred_file, "<Credential>\n");
	
	fprintf(cred_file, "\t<Name>%s</Name>\n", cred->get_name().c_str());
	
	fprintf(cred_file, "\t<IssuerID>%s</IssuerID>\n", cred->get_issuer().c_str());
	
	fprintf(cred_file, "\t<Id>%u</Id>\n", cred->get_credential_id());
	
	fprintf(cred_file, "\n");
	
	fprintf(cred_file, "\t<Secret>%s</Secret>\n", cred->get_silvia_credential()->get_secret().int_rep().c_str());
	
	fprintf(cred_file, "\n");
	
	fprintf(cred_file, "\t<Attributes>\n");
	
	std::vector<std::string> attr_names = cred->get_attribute_names();
	std::vector<silvia_attribute*> attr_values = cred->get_silvia_credential()->get_attributes();
	
	std::vector<std::string>::iterator attr_name_it = attr_names.begin();
	std::vector<silvia_attribute*>::iterator attr_value_it = attr_values.begin();
	
	while ((attr_name_it != attr_names.end()) && (attr_value_it != attr_values.end()))
	{
		fprintf(cred_file, "\t\t<Attribute type=\"int\">\n");
		
		fprintf(cred_file, "\t\t\t<Name>%s</Name>\n", attr_name_it->c_str());
		
		fprintf(cred_file, "\t\t\t<Value>%s</Value>\n", (*attr_value_it)->int_rep().c_str());
		
		fprintf(cred_file, "\t\t</Attribute>\n");
		
		attr_name_it++;
		attr_value_it++;
	}
	
	fprintf(cred_file, "\t</Attributes>\n");
	
	// Dirty way to convert A,e,v to integers without having to pull in libgmp
	silvia_integer_attribute A_int(cred->get_silvia_credential()->get_A());
	silvia_integer_attribute e_int(cred->get_silvia_credential()->get_e());
	silvia_integer_attribute v_int(cred->get_silvia_credential()->get_v());
	
	fprintf(cred_file, "\n");
	
	fprintf(cred_file, "\t<Signature>\n");
	
	fprintf(cred_file, "\t\t<A>%s</A>\n", A_int.int_rep().c_str());
	
	fprintf(cred_file, "\t\t<e>%s</e>\n", e_int.int_rep().c_str());
	
	fprintf(cred_file, "\t\t<v>%s</v>\n", v_int.int_rep().c_str());
	
	fprintf(cred_file, "\t</Signature>\n");
	
	fprintf(cred_file, "</Credential>\n");
	
	fclose(cred_file);
	
	return true;
}
