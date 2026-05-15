#pragma once
#include "ast.h"

NodePtr parseExpression();
NodePtr parseTerm();
NodePtr parseFactor();
NodePtr parseComparison();

NodePtr parseStatement();
NodePtr parseIfStmt();
NodePtr parseWhileStmt();
NodePtr parseDoWhileStmt();
NodePtr parseSwitchStmt();
NodePtr parseBlock();
NodePtr parseReturn();
NodePtr parseBreak();
NodePtr parseAssignOrExpr();

NodePtr parseProgram();
