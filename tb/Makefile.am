AM_CFLAGS = -Wall -Werror
AM_LDFLAGS = -rdynamic
AM_CPPFLAGS = -I$(top_srcdir)/common -I$(top_srcdir)/tb

nobase_include_HEADERS = paf/tb.h
INTERNAL_INCLUDES = hwcap.h \
                    common.h

dist_man3_MANS = doc/libpaf-tb.3

TESTS = test_tb
test_tb_SOURCES = tests/test_tb.c

check_PROGRAMS = $(TESTS)
