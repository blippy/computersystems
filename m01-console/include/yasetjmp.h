#pragma once
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2020  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifdef __cplusplus
extern "C" {
#endif

#if __SIZEOF_POINTER__ == 4
typedef u32	jmp_buf[11];
#else
typedef u64	jmp_buf[14];
#endif

int setjmp (jmp_buf env);
_Noreturn void longjmp (jmp_buf env, int val);

#ifdef __cplusplus
}
#endif
