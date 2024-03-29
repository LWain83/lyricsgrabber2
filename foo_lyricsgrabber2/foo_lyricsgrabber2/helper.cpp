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

void string_helper::convert_to_lower_case(const pfc::string_base& p_source, pfc::string_base& p_out)
{
	for (int i = 0; i < p_source.get_length(); i++)
	{
		p_out.add_char(tolower(p_source[i]));
	}
}

void string_helper::convert_to_lower_case(pfc::string_base& p_source)
{
	pfc::string8_fast temp;
	for (int i = 0; i < p_source.get_length(); i++)
	{
		temp.add_char(tolower(p_source[i]));
	}
	p_source = temp;
}

void string_helper::remove_char(pfc::string_base& p_source, const char p_remove)
{
	pfc::string8_fast temp;
	for (int i = 0; i < p_source.get_length(); i++)
	{
		if (p_source[i] == p_remove)
			continue;
		temp.add_char(p_source[i]);
	}
	p_source = temp;
}

void string_helper::remove_non_alphanumeric(pfc::string_base& p_source)
{
	pfc::string8_fast temp;

	for (int i = 0; i < p_source.get_length(); i++)
	{
		if (p_source[i] < 48)
			continue;
		if (p_source[i] > 57 && p_source[i] < 65)
			continue;
		if (p_source[i] > 90 && p_source[i] < 97)
			continue;
		if (p_source[i] > 123)
			continue;
		temp.add_char(p_source[i]);
	}

	p_source = temp;
}

void string_helper::remove_non_alphanumeric_keep_space(pfc::string_base& p_source)
{
	pfc::string8_fast temp;

	for (int i = 0; i < p_source.get_length(); i++)
	{
		if (p_source[i] < 48 && p_source[i] != 32)
			continue;
		if (p_source[i] > 57 && p_source[i] < 65)
			continue;
		if (p_source[i] > 90 && p_source[i] < 97)
			continue;
		if (p_source[i] > 123)
			continue;
		temp.add_char(p_source[i]);
	}

	p_source = temp;
}

pfc::string_list_impl * string_helper::split_lines(const pfc::string_base& p_source)
{
	if (p_source.get_length() > 0)
	{
		pfc::string_list_impl* list = new pfc::string_list_impl;
		pfc::string8_fast temp;
		int start = 0;
		int index = p_source.find_first('\n', start);

		while (index != -1)
		{
			temp = pfc::string8_fast(p_source.get_ptr() + start, index - start);

			list->add_item(temp);

			start = index + 1;
			index = p_source.find_first('\n', start);
		}
		temp = pfc::string8_fast(p_source.get_ptr() + start);
		list->add_item(temp);
		return list;
	}
	return NULL;
}

void string_helper::remove_end_linebreaks(pfc::string8& p_source)
{
	int index;

	do 
	{
		index = p_source.find_first('\r', p_source.get_length()-1);
		if (index == -1)
		{
			index = p_source.find_first('\n', p_source.get_length()-1);
			if (index == -1)
				break;
		}
		p_source.remove_chars(index, 1);

	} while (true);
}

void string_helper::remove_beginning_linebreaks(pfc::string8& p_source)
{
	while (p_source.get_length() > 0)
	{
		if (p_source[0] == '\r' || p_source[0] == '\n')
		{
			p_source.remove_chars(0, 1);
		}
		else
			break;
	}
}

void string_helper::remove_end(pfc::string8& p_source, const char toRemove)
{
	int index = p_source.find_first(toRemove, p_source.get_length()-1);

	while (index != -1)
	{
		p_source.remove_chars(index, 1);

		index = p_source.find_first(toRemove, p_source.get_length()-1);
	}
}

void string_helper::remove_beginning(pfc::string8& p_source, const char toRemove)
{
	while (p_source.get_length() > 0)
	{
		if (p_source[0] == toRemove)
			p_source.remove_chars(0,1);
		else
			break;
	}
}

#define min(a, b) (((a) < (b)) ? (a) : (b))

// finds the minimum of tree integers
int _min(int a, int b, int c) {
	return min(min(a, b), c);
}

// allocates a 2D array of integers
int **create_matrix(int Row, int Col) {
	int **array = new int*[Row];
	for(int i = 0; i < Row; ++i) {
		array[i] = new int[Col];
	}
	return array;
}

// deallocates memory
int **delete_matrix(int **array, int Row, int Col) {
	for(int i = 0; i < Row; ++i) {
		delete array[i];
	}
	delete [] array;
	return array;
}

// computes the Levenshtein distance between two strings
// "x" represent the pattern and "y" represent the text
// "m" is the pattern length and "n" is the text length
int LD(const char *x, unsigned int m, const char *y, unsigned int n) {
	// if the length of the second string is zero
	// then the distance between the two strings will
	// be equal to the length of the first string
	// and vis-versa
	// if the length of both strings is equal to zero
	// then the distance between this two strings will
	// simply be zero
	if (n == 0) {
		return m;
	} 
	else if (m == 0) {
		return n;
	}

	// creating a matrix of m+1 rows and n+1 columns
	int **matrix = create_matrix(m + 1, n + 1);

	// initialising the first row of the matrix
	for(unsigned int i = 0; i <= n; ++i) {
		matrix[0][i] = i; 
	}

	// initialising the first column of the matrix
	for(unsigned i = 0; i <= m; ++i) {
		matrix[i][0] = i; 
	}

	// complementary variables for computing the "Levenshtein distance"
	unsigned int above_cell, left_cell, diagonal_cell, cost;

	// starting the main process for computing 
	// the distance between the two strings "x" and "y"
	for(unsigned i = 1; i <= m; ++i) {
		for(unsigned int j = 1; j <= n; ++j) {
			// if the current two characters
			// of both strings are the same
			// then, the corresponding cost value
			// will be zero,otherwise it will be 1
			if (x[i-1] == y[j-1]) {
				cost = 0;
			} 
			else {
				cost = 1;
			}
			// current cell of the matrix: matrix[i][j]

			// finds the above cell to the current cell
			above_cell = matrix[i-1][j];

			// finds the left cell to the current cell
			left_cell = matrix[i][j-1];

			// finds the diagonally above cell to the current cell
			diagonal_cell = matrix[i-1][j-1];

			// computes the current value of the "edit distance" and place
			// the result into the current matrix cell
			matrix[i][j] = _min(above_cell + 1, left_cell + 1, diagonal_cell + cost);
		}
	}
	// placing the final result into a variable
	unsigned int result = matrix[m][n];
	// freeing memory that has been used
	// for the "matrix variable"
	delete_matrix(matrix, m + 1, n + 1);
	// returning result of the search
	return result;
}
