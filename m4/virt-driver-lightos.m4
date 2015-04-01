dnl The lightos driver
dnl
dnl Copyright (C) 2014 Roman Bogorodskiy
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library.  If not, see
dnl <http://www.gnu.org/licenses/>.
dnl

AC_DEFUN([LIBVIRT_DRIVER_CHECK_LIGHTOS],[
	AC_ARG_WITH([lightos],[
		AS_HELP_STRING([--with-lightos],
			[add lightos driver support @<:@default=yes@:>@])])
	m4_divert_text([DEFAULTS],[with_lightos=yes])
	if test "$with_lightos" = "yes";then
            AC_DEFINE_UNQUOTED([WITH_LIGHTOS],1,[whether Lightos driver is enabled])

	    AC_DEFINE_UNQUOTED([LIGHTOS],["$LIGHTOS"],[Location of the lightos tools])
	fi

	AM_CONDITIONAL([WITH_LIGHTOS],[test "$with_lightos" = "yes"])
])


AC_DEFUN([LIBVIRT_DRIVER_RESULT_LIGHTOS],[
    AC_MSG_NOTICE([    Lightos: $with_lightos])
])
