/*
    Copyright (C) 2025-2026 erysdren (it/its)

    This file is part of WifeRadiant.

    WifeRadiant is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    WifeRadiant is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with WifeRadiant.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>

class Tokenizer {
private:
	std::string m_input;
	std::string m_delims;
	size_t m_current_pos = 0;
	size_t m_current_line = 0;
	bool m_escapes = true;
	bool m_crossline = true;
	const char* m_error = nullptr;

	static constexpr bool isSpace(int c) {
		if (c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n' || c == '\f') {
			return true;
		} else {
			return false;
		}
	}

	static constexpr bool isNewline(int c) {
		if (c == '\r' || c == '\n') {
			return true;
		} else {
			return false;
		}
	}

	static constexpr bool isQuote(int c) {
		if (c == '\"') {
			return true;
		} else {
			return false;
		}
	}

	bool isDelim(int c) {
		if (auto found = m_delims.find(c); found != m_delims.npos) {
			return true;
		} else {
			return false;
		}
	}

	bool atEnd() {
		return m_current_pos >= m_input.length();
	}

	size_t remaining() {
		if (atEnd()) {
			return 0;
		} else {
			return m_input.length() - m_current_pos;
		}
	}

	bool peekChar(char *c) {
		if (atEnd()) {
			return false;
		} else {
			*c = m_input[m_current_pos];
			return true;
		}
	}

	bool peekCharBackwards(char *c) {
		if (m_current_pos == 0) {
			return false;
		} else {
			*c = m_input[m_current_pos - 1];
			return true;
		}
	}

	void eatLine() {
		char c;
		while (peekChar(&c) && !isNewline(c)) {
			m_current_pos++;
		}
	}

	void eatWhitespace() {
		char c;
		while (peekChar(&c) && isSpace(c)) {
			// iterate line counter
			if (!m_crossline && isNewline(c)) {
				return;
			} else if (isNewline(c)) {
				m_current_line++;
			}
			m_current_pos++;
		}
	}

	bool eatComments() {
		// comments need at least 2 chars
		if (remaining() < 2)
			return true;

		// line comments
		while (strncmp(m_input.c_str() + m_current_pos, "//", 2) == 0) {
			eatLine();
			eatWhitespace();
			// exit if reached eof
			if (remaining() < 2) {
				return true;
			}
		}

		// line comments
		while (strncmp(m_input.c_str() + m_current_pos, "#", 1) == 0) {
			eatLine();
			eatWhitespace();
			// exit if reached eof
			if (remaining() < 2) {
				return true;
			}
		}

		// block comments
		while (strncmp(m_input.c_str() + m_current_pos, "/*", 2) == 0) {
			m_current_pos += 2;

			while (1) {
				// error if reached eof
				if (remaining() < 2) {
					setError("overflowed");
					return false;
				}

				if (strncmp(&m_input[m_current_pos], "*/", 2) == 0) {
					m_current_pos += 2;
					break;
				}

				m_current_pos++;
			}

			eatWhitespace();

			// exit if reached eof
			if (remaining() < 2) {
				return true;
			}
		}

		return true;
	}

	void setError(const char* error) {
		m_error = error;
	}

public:
	Tokenizer(std::string input, const char* delims, bool escapes = true) : m_input(input), m_delims(delims), m_escapes(escapes) { }
	Tokenizer(const char* ptr, size_t len, const char* delims, bool escapes = true) : m_input(ptr, len), m_delims(delims), m_escapes(escapes) { }
	Tokenizer(const char* start, const char* end, const char* delims, bool escapes = true) : m_input(start, end), m_delims(delims), m_escapes(escapes) { }
	~Tokenizer() = default;

	class Token {
	friend Tokenizer;
	private:
		std::string m_token;
		bool m_quoted = false;
		bool m_lineStart = false;
	protected:
		Token() { };
	public:
		operator std::string() const { return m_token; }
		char& operator[](size_t n) { return m_token[n]; }
		const char& operator[](size_t n) const { return m_token[n]; }
		explicit operator bool() const { return !m_token.empty(); }
		bool isQuoted() const { return m_quoted; }
		bool isLineStart() const { return m_lineStart; }
	};

	bool getCrossLine() const { return m_crossline; }
	void setCrossLine(bool crossline) { m_crossline = crossline; }

	bool getEscapes() const { return m_escapes; }
	void setEscapes(bool escapes) { m_escapes = escapes; }

	const char* getError() const { return m_error; }

	void skipLine() {
		eatLine();
	}

	Token getToken() {
		Token token;
		char c, cc;

		// end of buffer
		if (!peekChar(&c)) {
			return token;
		}

		// check if we can cross line
		if (!m_crossline && isNewline(c)) {
			return token;
		}

		// eating whitespace never fails
		eatWhitespace();

		// eating comments might fail if c-style comment is unclosed
		if (!eatComments()) {
			return token;
		}

		// might be at the end of the buffer
		if (!peekChar(&c)) {
			return token;
		}

		// check if we're at the start of a line
		if (peekCharBackwards(&cc) && isNewline(cc)) {
			token.m_lineStart = true;
		}

		// deliminators get their own token
		if (isDelim(c)) {
			token.m_token += c;
			m_current_pos++;
			return token;
		}

		// copy token up to whitespace
		while (peekChar(&c)) {
			if (!token.m_quoted && isQuote(c)) {
				// start of quoted token
				token.m_quoted = true;
				m_current_pos++;
			} else if (token.m_quoted && isQuote(c)) {
				// end of quoted token
				m_current_pos++;
				break;
			} else if (token.m_quoted && isNewline(c)) {
				// newline breaking quoted token
				setError("line breaking inside quoted token");
				break;
			} else if (!token.m_quoted && isSpace(c)) {
				// end of unquoted token
				break;
			} else if (!token.m_quoted && isDelim(c)) {
				// deliminator means end of token
				break;
			} else if (token.m_quoted && m_escapes && c == '\\') {
				// parse escape sequence
				m_current_pos++;

				// error if there's no second char
				if (!peekChar(&c)) {
					setError("overflowed");
					break;
				}

				switch (c) {
					case '\\': token.m_token += '\\'; break;
					case 'n': token.m_token += '\n'; break;
					case 't': token.m_token += '\t'; break;
					case '"': token.m_token += '"'; break;
					default: {
						// copy backslash
						token.m_token += '\\';

						// copy second character
						token.m_token += c;
						break;
					}
				}
				m_current_pos++;
			} else {
				// copy character
				token.m_token += c;
				m_current_pos++;
			}
		}

		return token;
	}
};
