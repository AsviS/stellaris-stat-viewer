/* parser.h: A parser for PDS script/declaration files (header file)
 *
 * Copyright 2019 Adrian "ArdiMaster" Welcker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifndef STELLARIS_STAT_VIEWER_PARSER_H
#define STELLARIS_STAT_VIEWER_PARSER_H

#include <forward_list>

#include <QtCore/QFileInfo>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QString>
class QFile;
class QTextStream;

namespace Parsing {
	enum TokenType {
		TT_STRING,
		TT_OBRACE,
		TT_CBRACE,
		TT_EQUALS,
		TT_BOOL,
		TT_INT,
		TT_DOUBLE,
		TT_LT,
		TT_GT,
		TT_NONE
	};

	struct Token {
		unsigned long line;
		unsigned long firstChar;
		TokenType type;
		union {
			char String[64];
			bool Bool;
			qint64 Int;
			double Double;
		} tok;
	};
	
	enum class FileType {
		SaveFile,
		GameFile,
		ModFile,
		NoFile
	};

	enum NodeType {
		NT_INDETERMINATE = 0,
		NT_COMPOUND,
		NT_STRING,
		NT_BOOL,
		NT_INT,
		NT_DOUBLE,
		NT_INTLIST,
		NT_INTLIST_MEMBER,
		NT_DOUBLELIST,
		NT_DOUBLELIST_MEMBER,
		NT_COMPOUNDLIST,
		NT_COMPUNDLIST_MEMBER,
		NT_STRINGLIST,
		NT_STRINGLIST_MEMBER,
		NT_BOOLLIST,
		NT_BOOLLIST_MEMBER,
		NT_EMPTY
	};

	enum RelationType {
		RT_NONE = 0,
		RT_EQ,
		RT_GT,
		RT_GE,
		RT_LT,
		RT_LE
	};

	struct AstNode {
		/** Merge 'other' into this tree
		 *
		 * All children of 'other' will become children of this.
		 *
		 * If this and other are not of the same type, nothing will happen.
		 * Also works if 'this' and/or 'other' does not have any children.
		 *
		 * @param other The tree to merge into this tree.
		 */
		void merge(AstNode *other);
		/** Finds the first child of this node with the given name.
		 *
		 * @param name The name of the child to search for.
		 * @return The child searched for, or nullptr if no child of that name exists.
		 */
		AstNode *findChildWithName(const char *name) const;
		qint64 countChildren() const;

		char myName[64] = {'\0'};
		NodeType type = NT_INDETERMINATE;
		AstNode *nextSibling = nullptr;
		RelationType relation = RT_NONE;
		union NodeValue {
			char Str[64];
			bool Bool;
			qint64 Int;
			double Double;
			struct { AstNode *firstChild; AstNode *lastChild; };
		} val = {{'\0'}};
	};

	void printParseTree(const AstNode *tree, int indent = 0, bool toplevel = true);

	enum ParseErr {
		PE_NONE,
		PE_INVALID_IN_COMPOUND,
		PE_INVALID_AFTER_NAME,
		PE_INVALID_AFTER_EQUALS,
		PE_INVALID_AFTER_RELATION,
		PE_INVALID_AFTER_OPEN,
		PE_INVALID_COMBO_AFTER_OPEN,
		PE_INVALID_IN_INT_LIST,
		PE_INVALID_IN_DOUBLE_LIST,
		PE_INVALID_IN_COMPOUND_LIST,
		PE_INVALID_IN_STRING_LIST,
		PE_INVALID_IN_BOOL_LIST,
		PE_UNEXPECTED_END,
		PE_TOO_MANY_CLOSE_BRACES,
		LE_INVALID_INT,
		LE_INVALID_DOUBLE,
		PE_CANCELLED
	};

	QString getErrorDescription(ParseErr etype);

	struct ParserError {
		ParseErr etype;
		Token erroredToken;
	};

	class Parser : public QObject {
		Q_OBJECT
	public:
		explicit Parser(QString *text, QObject *parent = nullptr);
		Parser(const QFileInfo &fileInfo, FileType ftype, QObject *parent = nullptr);
		Parser(QTextStream *stream, QString filename, FileType ftype, QObject *parent = nullptr);
		~Parser() override;
		AstNode *parse();
		void cancel();
		ParserError getLatestParserError() const;

	signals:
		void progress(Parser *parser, qint64 current, qint64 total);

	private:
		Token getNextToken();
		int lex(int atLeast = 0);
		TokenType lookahead(int n);
		AstNode *createNode();

		bool lexerDone = false;
		bool shouldCancel = false;
		bool shouldDeleteStream;

		FileType fileType;
		QFile *file;
		QString filename;
		QQueue<Token> lexQueue;
		QTextStream *stream;
		qint64 totalProgress = 0;
		qint64 totalSize;
		std::forward_list<AstNode> allCreatedNodes;

		const int queueCapacity = 50;
		unsigned long line = 1;
		unsigned long charPos = 0;

		ParserError latestParserError{PE_NONE, {0, 0, TT_NONE, {{0}}}};

		unsigned int lexCalls1 = 1;
		unsigned int lexCalls2 = 1;
	};
}

#endif //STELLARIS_STAT_VIEWER_PARSER_H
