#pragma once

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
//! Helpers

class string_trim_left {
public:
	inline string_trim_left(const char * src)
	{
		t_size pos;

		for (pos = 0; pos < strlen(src); pos++)
			if (!isspace(src[pos]))
				break;
		
		m_data = src + pos;
	}
	inline operator const char * () const {return m_data.get_ptr();}
	inline const char * get_ptr() const {return m_data.get_ptr();}
	inline t_size get_length() const {return m_data.get_length();}
private:
	pfc::string8_fast m_data;
};

class string_trim_right {
public:
	inline string_trim_right(const char * src)
	{
		t_size pos;

		for (pos = strlen(src) - 1; pos >=0; pos--)
			if (!isspace(src[pos]))
				break;

		m_data.set_string(src, pos + 1);
	}
	inline operator const char * () const {return m_data.get_ptr();}
	inline const char * get_ptr() const {return m_data.get_ptr();}
	inline t_size get_length() const {return m_data.get_length();}
private:
	pfc::string8_fast m_data;
};

class string_trim {
public:
	inline string_trim(const char * src)
	{
		m_data = string_trim_left(src);
		m_data = string_trim_right(m_data);
	}
	inline operator const char * () const {return m_data.get_ptr();}
	inline const char * get_ptr() const {return m_data.get_ptr();}
	inline t_size get_length() const {return m_data.get_length();}
private:
	pfc::string8_fast m_data;
};

class string_helper
{
public:
	static void convert_to_lower_case(const pfc::string_base& p_source, pfc::string_base& p_out);
	static void convert_to_lower_case(pfc::string_base& p_source);
	static void remove_char(pfc::string_base& p_source, const char p_remove);
	static void remove_non_alphanumeric(pfc::string_base& p_source);
	static void remove_non_alphanumeric_keep_space(pfc::string_base& p_source);
	static pfc::string_list_impl * split_lines(const pfc::string_base& p_source);
	static void remove_end_linebreaks(pfc::string8& p_source);
	static void remove_beginning_linebreaks(pfc::string8& p_source);
	static void remove_end(pfc::string8& p_source, const char toRemove);

private:
	string_helper() {};
};

class lev_distance {
public:
	inline lev_distance(const char * s, const char * t, bool ignore_case = true) throw()
	{
		lev_distance(s, strlen(s), t, strlen(t), ignore_case);
	}

	lev_distance(const char * s, t_size len_s, const char * t, t_size len_t, bool ignore_case) throw()
	{
		t_size m = len_s;
		t_size n = len_t;

		t_size ** T = new t_size * [m + 1];

		for (t_size i = 0; i < n; i++)
			T[i] = new t_size[n + 1];

		T[0][0] = 0;

		for (t_size i = 0; i < n; i++)
			T[0][i + 1] = T[0][i] + 1;

		for (t_size i = 0; i < m; i++)
		{
			T[i + 1][0] = T[i][0] + 1;

			for (t_size j = 0; j < n; j++)
			{
				T[i + 1][j + 1] = min3(
					T[i][j] + sub(s, i, t, j, ignore_case),
					T[i][j+1] + 1,
					T[i+1][j] + 1);
			}
		}

		m_data = T[m][n];

		// Free carefully
		for(t_size i=0; i < m+1; i++)
			delete [] T[i];

		delete [] T;
	}
	inline operator t_size () const { return m_data; }
	inline t_size get_value() const { return m_data; }
private:
	inline t_size sub(const char * s, int si, const char * t, int ti, bool ignore_case) throw()
	{
		if (ignore_case)
			return (pfc::ascii_tolower(s[si]) == pfc::ascii_tolower(t[ti])) ? 0 : 1;
		else
			return (s[si] == t[ti]) ? 0 : 1;
	}

	inline t_size min3(t_size item1, t_size item2, t_size item3) throw()
	{
		return pfc::min_t(pfc::min_t(item1, item2), item3);
	}

	t_size m_data;
};

void convert_html_to_plain(pfc::string_base & p_out) throw();
void decode_html_entities(pfc::string_base & p_out) throw();

#define min(a, b) (((a) < (b)) ? (a) : (b))

// finds the minimum of tree integers
int _min(int a, int b, int c);

// allocates a 2D array of integers
int **create_matrix(int Row, int Col);

// deallocates memory
int **delete_matrix(int **array, int Row, int Col);

// computes the Levenshtein distance between two strings
// "x" represent the pattern and "y" represent the text
// "m" is the pattern length and "n" is the text length
int LD(const char *x, unsigned int m, const char *y, unsigned int n);