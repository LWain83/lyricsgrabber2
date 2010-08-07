#include "stdafx.h"
#include "helper.h"
#include "deelx.h"
#include <string>

struct tagEntities
{
	char * key;
	unsigned val;
};

const tagEntities entities[] = 
{
	{ "quot", 34 },
	{ "amp", 38 },
	{ "lt", 60 },
	{ "gt", 62 },
	{ "nbsp", ' ' },
	{ "iexcl", 161 },
	{ "cent", 162 },
	{ "pound", 163 },
	{ "curren", 164 },
	{ "yen", 165 },
	{ "brvbar", 166 },
	{ "sect", 167 },
	{ "uml", 168 },
	{ "copy", 169 },
	{ "ordf", 170 },
	{ "laquo", 171 },
	{ "not", 172 },
	{ "shy", 173 },
	{ "reg", 174 },
	{ "macr", 175 },
	{ "deg", 176 },
	{ "plusmn", 177 },
	{ "sup2", 178 },
	{ "sup3", 179 },
	{ "acute", 180 },
	{ "micro", 181 },
	{ "para", 182 },
	{ "middot", 183 },
	{ "cedil", 184 },
	{ "sup1", 185 },
	{ "ordm", 186 },
	{ "raquo", 187 },
	{ "frac14", 188 },
	{ "frac12", 189 },
	{ "frac34", 190 },
	{ "iquest", 191 },
	{ "Agrave", 192 },
	{ "Aacute", 193 },
	{ "Acirc", 194 },
	{ "Atilde", 195 },
	{ "Auml", 196 },
	{ "ring", 197 },
	{ "AElig", 198 },
	{ "Ccedil", 199 },
	{ "Egrave", 200 },
	{ "Eacute", 201 },
	{ "Ecirc", 202 },
	{ "Euml", 203 },
	{ "Igrave", 204 },
	{ "Iacute", 205 },
	{ "Icirc", 206 },
	{ "Iuml", 207 },
	{ "ETH", 208 },
	{ "Ntilde", 209 },
	{ "Ograve", 210 },
	{ "Oacute", 211 },
	{ "Ocirc", 212 },
	{ "Otilde", 213 },
	{ "Ouml", 214 },
	{ "times", 215 },
	{ "Oslash", 216 },
	{ "Ugrave", 217 },
	{ "Uacute", 218 },
	{ "Ucirc", 219 },
	{ "Uuml", 220 },
	{ "Yacute", 221 },
	{ "THORN", 222 },
	{ "szlig", 223 },
	{ "agrave", 224 },
	{ "aacute", 225 },
	{ "acirc", 226 },
	{ "atilde", 227 },
	{ "auml", 228 },
	{ "aring", 229 },
	{ "aelig", 230 },
	{ "ccedil", 231 },
	{ "egrave", 232 },
	{ "eacute", 233 },
	{ "ecirc", 234 },
	{ "euml", 235 },
	{ "igrave", 236 },
	{ "iacute", 237 },
	{ "icirc", 238 },
	{ "iuml", 239 },
	{ "ieth", 240 },
	{ "ntilde", 241 },
	{ "ograve", 242 },
	{ "oacute", 243 },
	{ "ocirc", 244 },
	{ "otilde", 245 },
	{ "ouml", 246 },
	{ "divide", 247 },
	{ "oslash", 248 },
	{ "ugrave", 249 },
	{ "uacute", 250 },
	{ "ucirc", 251 },
	{ "uuml", 252 },
	{ "yacute", 253 },
	{ "thorn", 254 },
	{ "yuml", 255 },
	{ NULL, 0 },
};

void convert_html_to_plain(pfc::string_base & p_out)
{
	char * tmp = NULL;
	CRegexpT<char> regexp;

	// '\r' or '\n'
	regexp.Compile("(?<=>\\s*)[\\r\\n]+");
	tmp = regexp.Replace(p_out, "");
	p_out.set_string(tmp);
	regexp.ReleaseString(tmp);

	// '\r' or '\n'
	regexp.Compile("[\\r\\n]+");
	tmp = regexp.Replace(p_out, " ");
	p_out.set_string(tmp);
	regexp.ReleaseString(tmp);
	
	// '\t' to null
	regexp.Compile("\\t");
	tmp = regexp.Replace(p_out, "");
	p_out.set_string(tmp);
	regexp.ReleaseString(tmp);

	// Remove repeated spaces
	//regexp.Compile("(\s)+");
	//tmp = regexp.Replace(p_out, " ");
	//p_out = tmp;
	//regexp.ReleaseString(tmp);

	// br, tr, div, p, li to \n
	regexp.Compile("</?\\s*(br|p|div|tr|li).*?/?>", IGNORECASE);
	tmp = regexp.Replace(p_out, "\r\n");
	p_out.set_string(tmp);
	regexp.ReleaseString(tmp);

	// anything thats enclosed inside '<' and '>'
	regexp.Compile("[\\r\\n]?<[^>]*>[\\r\\n]?", SINGLELINE);
	tmp = regexp.Replace(p_out, "");
	p_out.set_string(tmp);
	regexp.ReleaseString(tmp);

	decode_html_entities(p_out);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

void decode_html_entities(pfc::string_base & p_out)
{
	unsigned nCount = 0;
	std::string buf(p_out);
	std::string entity;
	const char * ptr = p_out.get_ptr();
	const char * pEnd;

	ptr = strchr(ptr, '&');

	if (ptr == NULL)
		return;

	nCount = static_cast<unsigned>(ptr - p_out.get_ptr());

	while (*ptr)
	{
		if (*ptr == '&' && (pEnd = strchr(ptr, ';')))
		{
			entity.assign(ptr + 1, pEnd);

			// &#20; for example
			if (!entity.empty() && entity[0] == '#')
			{
				unsigned c;
				t_size bytes;
				char target[6];

				entity.erase(0, 1);
				c = pfc::atoui_ex(entity.c_str(), entity.length());
				bytes = pfc::utf8_encode_char(c, target);

				for (t_size j = 0; j < bytes; j++)
				{
					buf[nCount++] = target[j];
				}

				ptr = pEnd + 1;
				continue;
			}
			else
			{
				// &nbsp; for example
				for (int i = 0; entities[i].key != NULL; ++i)
				{
					if (_stricmp(entity.c_str(), entities[i].key) == 0)
					{
						unsigned c = entities[i].val;
						t_size bytes;
						char target[6];

						c = entities[i].val;
						bytes = pfc::utf8_encode_char(c, target);

						for (t_size j = 0; j < bytes; j++)
						{
							buf[nCount++] = target[j];
						}

						ptr = pEnd + 1;
						break;
					}
				}

				continue;
			}
		}

		buf[nCount] = *ptr;
		nCount++;
		ptr++;
	}

	buf.erase(nCount);
	p_out = buf.c_str();
}
