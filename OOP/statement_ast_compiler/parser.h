#pragma once
#include "ast.h"

// ── Expression parser 
NodePtr parseExpression();
NodePtr parseTerm();
NodePtr parseFactor();
NodePtr parseComparison();   

// ── Statement parser 
NodePtr parseStatement();    
NodePtr parseIfStmt();
NodePtr parseWhileStmt();
NodePtr parseAssignOrExpr(); 
NodePtr parseBlock();      
NodePtr parseReturn();


NodePtr parseProgram();
