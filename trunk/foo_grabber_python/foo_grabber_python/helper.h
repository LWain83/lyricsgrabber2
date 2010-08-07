#pragma once

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

class lev_distance {
public:
	inline lev_distance(const char * s, const char * t, bool ignore_case = true)
	{ lev_distance(s, strlen(s), t, strlen(t), ignore_case); }
	lev_distance(const char * s, t_size len_s, const char * t, t_size len_t, bool ignore_case) 
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
	inline t_size sub(const char * s, int si, const char * t, int ti, bool ignore_case)
	{
		if (ignore_case)
			return (pfc::ascii_tolower(s[si]) == pfc::ascii_tolower(t[ti])) ? 0 : 1;
		else
			return (s[si] == t[ti]) ? 0 : 1;
	}
	inline t_size min3(t_size item1, t_size item2, t_size item3)
	{
		return pfc::min_t(pfc::min_t(item1, item2), item3);
	}
	t_size m_data;
};
