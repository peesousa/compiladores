#include "token.h"

const char* token_type_to_string(TokenType type){
  switch (type) {
    case TOKEN_BEGIN: return "BEGIN";
    case TOKEN_END: return "END";
    
    case TOKEN_INT: return "INT";
    case TOKEN_FLOAT: return "FLOAT";

    case TOKEN_IF: return "IF";
    case TOKEN_THEN: return "THEN";
    case TOKEN_ENDIF: return "ENDIF";
    case TOKEN_ELSE: return "ELSE";
    case TOKEN_ENDELSE: return "ENDELSE";

    case TOKEN_WHILE: return "WHILE";
    case TOKEN_DO: return "DO";
    case TOKEN_ENDWHILE: return "ENDWHILE";

    case TOKEN_PRINT: return "PRINT";
    case TOKEN_SCAN: return "SCAN";

    case TOKEN_FUNCTION: return "FUNCTION";
    case TOKEN_PROCEDURE: return "PROCEDURE";
    case TOKEN_RETURN: return "RETURN";

    case TOKEN_LPAREN: return "LPAREN";
    case TOKEN_RPAREN: return "RPAREN";
    case TOKEN_COMMA: return "COMMA";
    case TOKEN_SEMICOLON: return "SEMICOLON";

    case TOKEN_PLUS: return "PLUS";
    case TOKEN_MINUS: return "MINUS";
    case TOKEN_ASTERISK: return "ASTERISK";
    case TOKEN_SLASH: return "SLASH";

    case TOKEN_ASSIGN: return "ASSIGN";

    case TOKEN_EQ: return "EQ";
    case TOKEN_LT: return "LT";
    case TOKEN_LTE: return "LTE";
    case TOKEN_GT: return "GT";
    case TOKEN_GTE: return "GTE";
    case TOKEN_NEQ: return "NEQ";

    case TOKEN_IDENTIFIER: return "IDENTIFIER";

    case TOKEN_INTEGER_LITERAL: return "INTEGER_LITERAL";
    case TOKEN_FLOAT_LITERAL: return "FLOAT_LITERAL";

    case TOKEN_EOF: return "EOF";
    case TOKEN_UNKNOWN: return "UNKNOWN";

    default: return "INVALID_TOKEN_TYPE";
  }
}
