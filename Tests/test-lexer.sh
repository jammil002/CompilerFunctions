#!/bin/bash

# Lexer test input
cat <<EOF | ../lexer
int x = 10;
write x;
EOF

# Check the output for expected tokens
# Add your validation logic here, e.g., grep for expected tokens
