#!/bin/bash -e
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color
LIMIT=129
WARNINGS=$(doxygen |&  grep "warning" | wc -l)
# If no warnings, exit with 0
if [ "$WARNINGS" -eq 0 ]; then
    echo -e "${GREEN}No DOXYGEN detected${NC}"
    exit 0
fi
echo -e "Doxygen warnings: ${GREEN}$WARNINGS${NC}"

if [ "$WARNINGS" -gt "$LIMIT" ]; then
    echo -e "Doxygen warnings exceed ${RED}$LIMIT${NC}, failing build."
    exit $WARNINGS
fi
