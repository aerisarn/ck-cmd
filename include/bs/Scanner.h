// Scanner Class for C++ - Scanner++

// Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014
// scannerpp.sourceforge.net
//
// This file is part of the Scanner++ Library v0.98.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef SCANNERPP_H_
#define SCANNERPP_H_

#include<iostream>
#include<istream>
#include<string>
#include<sstream>
#include<algorithm>

#include<vector>
#include<map>

#include <stdlib.h>

namespace scannerpp
{

	class Scanner
	{
	private:
		istringstream input;

	public:

		Scanner(const string& input) : input(input) {}

		virtual ~Scanner() {}

		int nextInt() {
			int i;
			input >> i;
			nextLine();
			return i;
		}
		bool hasNextLine() { return !input.eof() && !input.fail() && !(EOF == input.peek()); }
		string nextLine() {
			string line;
			getline(input, line);
			return line;
		}
	};
}
#endif /*SCANNERPP_H_*/
