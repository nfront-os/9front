#!/bin/bash
# upas/smtp parsers, as the rules in sys/src/cmd/upas/smtp/mkfile build
# them (send and ml compile ../smtp/rfc822.tab.c too):
#	smtpd.tab.c:	yacc -o xxx smtpd.y; sed 's/yy/zz/g'
#	rfc822.tab.c rfc822.tab.h:	yacc -d -s rfc822 rfc822.y
set -e
ROOT=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
cd "$ROOT/sys/src/cmd/upas/smtp"
bash "$ROOT/scripts/yacc.sh" -o xxx smtpd.y
sed 's/yy/zz/g' < xxx > smtpd.tab.c
rm -f xxx
bash "$ROOT/scripts/yacc.sh" -d -s rfc822 rfc822.y
