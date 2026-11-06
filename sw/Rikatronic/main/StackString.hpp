#pragma once
#include <Arduino.h>
#include <string.h>
#include <stdlib.h>

// Zeichenkette mit fester Groesse komplett auf dem Stack (bzw. im Objekt).
// Kein malloc/free -> keine Heap-Fragmentierung. Ist der Platz zu klein, wird abgeschnitten.
template <size_t N>
class StackString
{
	public:
		StackString() { this->clear(); }
		StackString(const char* s) { this->clear(); this->append(s); }

		void clear() { this->len = 0; this->buf[0] = 0; }

		StackString& append(const char* s)
		{
			if (NULL == s) return *this;
			while (*s && this->len < N - 1)
			{
				this->buf[this->len++] = *s++;
			}
			this->buf[this->len] = 0;
			return *this;
		}
		StackString& append(char c)
		{
			if (this->len < N - 1)
			{
				this->buf[this->len++] = c;
				this->buf[this->len] = 0;
			}
			return *this;
		}
		StackString& append(long v)
		{
			char t[16];
			ltoa(v, t, 10);
			return this->append(t);
		}
		StackString& append(int v) { return this->append((long)v); }
		StackString& append(unsigned int v) { return this->append((long)v); }
		StackString& append(double v, int decimals = 2)
		{
			char t[24];
			dtostrf(v, 1, decimals, t);
			return this->append(t);
		}

		StackString& operator+=(const char* s) { return this->append(s); }
		StackString& operator+=(char c) { return this->append(c); }
		StackString& operator+=(long v) { return this->append(v); }
		StackString& operator+=(int v) { return this->append(v); }
		StackString& operator+=(double v) { return this->append(v); }

		const char* c_str() const { return this->buf; }
		size_t length() const { return this->len; }
		bool equals(const char* s) const { return 0 == strcmp(this->buf, s); }
		bool startsWith(const char* s) const { return 0 == strncmp(this->buf, s, strlen(s)); }
		bool endsWith(const char* s) const
		{
			size_t n = strlen(s);
			return n <= this->len && 0 == strcmp(this->buf + this->len - n, s);
		}
		void toLower()
		{
			for (size_t i = 0; i < this->len; i++)
			{
				if (this->buf[i] >= 'A' && this->buf[i] <= 'Z') this->buf[i] += 32;
			}
		}

	private:
		char buf[N];
		size_t len;
};
