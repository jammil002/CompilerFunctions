#!/bin/bash

# Parser test input
cat <<EOF | ../parser
int x = 10;
write x;
EOF

# Add checks for expected output or absence of syntax errors
