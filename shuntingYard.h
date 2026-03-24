//
// shuntingYard.h
//
#include <string>
#include <stdexcept>
#include <cmath>
#include "List.h"
#include "Stack.h"
#pragma once

class shuntingYard
{
public:
	class shuntingException : std::exception
	{
		std::wstring message;

	public:
		shuntingException(std::wstring errMessage) : message(errMessage) {}

		const char* what() const noexcept
		{
			return "Error";
		}

		const wchar_t* wwhat() const
		{
			return message.c_str();
		}
	};

private:
	pcwList rpnList;
	Stack<const wchar_t, cwList> stack;

	enum Assoc
	{
		LEFT,
		RIGHT
	};

	enum Expected
	{
		OPERATOR,
		OPERAND
	};

	struct internalOperatorHandler
	{
		struct OperStruct
		{
			wchar_t sign;
			int priority;
			Assoc associativity;
		};

		inline static OperStruct OPERATORS[] =
		{
			{L'+', 1, LEFT},
			{L'-', 1, LEFT},
			{L'*', 2, LEFT},
			{L'/', 2, LEFT},
			{L'^', 3, RIGHT},
			{L'~', 4, RIGHT},
		};

		static const OperStruct* get(const wchar_t sign)
		{
			for (const auto &opStuct : OPERATORS)
			{
				if (opStuct.sign == sign) return &opStuct;
			}
			return nullptr;
		}

		static const OperStruct* get(const wchar_t* token)
		{
			// Only one chared signs by now
			if (!token || token[0] == L'\0' || token[1] != L'\0') return nullptr;
			return get(token[0]);
		}
	};

	struct internalVariables
	{
		std::wstring name;
		double value;
	};

	List<internalVariables> variablesList_;

	const internalVariables* getVariable(const wchar_t* variableName)
	{
		for (auto &var : variablesList_)
		{
			if (var.name == variableName) return &var; // Can do this 'cause of basic_string == overload
		}
		return nullptr;
	}

	std::wstring getErrorTrace(const std::wstring &unsortExpression, int errorPos)
	{
		return L"->" + unsortExpression.substr(errorPos, 6) + L"...";
	}

	struct ErrorTokenContext
	{
		const std::wstring &errorMessage; // Only for temporary usage in processTokenError()
		pcwList &tokens;
		pcwList::Iterator fromToken;
		int tokenTraceCount;
	};

	void processTokenError(const ErrorTokenContext &etc)
	{
		std::wstring unsortExpression;
		auto tokensIter = etc.fromToken;
		int count = etc.tokenTraceCount;

		while (count != 0 && tokensIter != etc.tokens.end())
		{
			unsortExpression.append(*tokensIter++);
			--count;
		}

		std::wstring errorTrace = getErrorTrace(unsortExpression, 0);
		throw shuntingException(etc.errorMessage + errorTrace);
	}


public:
	using Variables = internalVariables;
	using OperatorHandler = internalOperatorHandler;

	//shuntingYard();
	//~shuntingYard();

	void setVariables(const List<internalVariables> &variablesList)
	{
		variablesList_ = variablesList;
	}

	std::wstring getRpnWString()
	{
		if (rpnList.empty()) return std::wstring();

		std::wstring outRpn;

		for (const auto &wch : rpnList)
		{
			outRpn.append(wch);
			outRpn.append(L" ");
		}
		outRpn.erase(outRpn.length() - 1, 1);

		return outRpn;
	}

	pcwList unsortToTokens(std::wstring &unsortExpression)
	{
		pcwList outTokenList;
		std::wstring buffer;

		int unsortIter = 0;
		size_t unsortLen = unsortExpression.length();
		while (unsortIter < unsortLen)
		{
			wchar_t wch = unsortExpression[unsortIter];

			if (wch == L' ' || wch == L'\t' || wch == L'\n' || wch == L'\r') // Delimiter
			{
				++unsortIter;
				continue;
			}

			if (internalOperatorHandler::get(wch) || wch == L'(' || wch == L')')     // Operator
			{
				wchar_t tempWString[] = { wch, L'\0' };
				outTokenList.push_back(tempWString);
				++unsortIter;
				continue;
			}

			if (iswalpha(wch))                                               // Variables
			{
				buffer.clear();
				while (unsortIter < unsortLen)
				{
					wch = unsortExpression[unsortIter];                      // Update wch in internal cycles

					if (iswalnum(wch))                                       // Like "x1", "x2"
					{
						buffer.push_back(wch);
					}
					else if (wch == L'.')                                    // "x.1", "x.x"
					{
						std::wstring errorTrace = getErrorTrace(unsortExpression, unsortIter);
						throw shuntingException(L"Переменная не может содержать точку " + errorTrace);
					}
					else                                                     // Parced whole variable
					{
						break;
					}
					++unsortIter;
				}
				outTokenList.push_back(buffer.c_str());
				continue;
			}

			if (iswdigit(wch))                                               // Numbers
			{
				buffer.clear();
				int dotCount = 0;

				while (unsortIter < unsortLen)
				{
					wch = unsortExpression[unsortIter];

					if (iswdigit(wch))
					{
						buffer.push_back(wch);
					}
					else if (wch == L'.')
					{
						++dotCount;
						if (dotCount > 1)
						{
							std::wstring errorTrace = getErrorTrace(unsortExpression, unsortIter);
							throw shuntingException(L"Слишком много точек в числе " + errorTrace);
						}
						buffer.push_back(wch);
					}
					else if (iswalpha(wch))                                  // "1a" not the variable
					{
						std::wstring errorTrace = getErrorTrace(unsortExpression, unsortIter);
						throw shuntingException(L"Число не может содержать буквы " + errorTrace);
					}
					else                                                     // Parced whole number
					{
						break;
					}
					++unsortIter;
				}
				outTokenList.push_back(buffer.c_str());
				continue;
			}

			std::wstring errorTrace = getErrorTrace(unsortExpression, unsortIter);
			throw shuntingException(L"Неизвестный символ " + errorTrace);
		}

		return outTokenList;
	}

	pcwList shuntToRpn(std::wstring &unsortExpression)
	{
		rpnList.clear();

		// Operators and operands in form of const wchar_t* pointer
		pcwList tokensList = unsortToTokens(unsortExpression);


		Expected expected = OPERAND;
		const wchar_t* prevToken = L"";

		auto tokenIter = tokensList.begin();
		while (tokenIter != tokensList.end())                     // Iterating on tokens
		{
			const wchar_t* token = *tokenIter;

			// Variable or number
			if (iswalnum(token[0]))
			{
				if (expected != OPERAND)
				{
					processTokenError({
						L"Ожидался операнд ",
						tokensList,
						tokenIter,
						6
						});
				}
				expected = OPERATOR;

				rpnList.push_back(token);
			}
			// Unary minus
			else if (wcscmp(token, L"-") == 0 &&
				(wcscmp(prevToken, L"") == 0 || internalOperatorHandler::get(prevToken) || wcscmp(prevToken, L"(") == 0))
			{
				if (expected != OPERAND)
				{
					processTokenError({
						L"Ожидался операнд ",
						tokensList,
						tokenIter,
						6
						});
				}
				expected = OPERAND;

				stack.push(L'~');
			}
			// Binary operator
			else if (internalOperatorHandler::get(token))
			{
				if (expected != OPERATOR)
				{
					processTokenError({
						L"Ожидался оператор ",
						tokensList,
						tokenIter,
						6
						});
				}
				expected = OPERAND;

				auto tokenOperator = internalOperatorHandler::get(token);
				while (!stack.empty())
				{
					auto stackOperator = internalOperatorHandler::get(stack.top());
					if (!stackOperator) break; // nullptr on get()

					bool shouldToPop = tokenOperator->priority < stackOperator->priority ||
									  (stackOperator->priority == tokenOperator->priority &&
									   stackOperator->associativity == LEFT &&
									   tokenOperator->associativity == LEFT);

					// Weaker operator couldn't lie on stronger
					// And if they are equal pop if they are both left associated
					// Because expression executes from left to right on left associated operators
					if (!shouldToPop) break;

					const wchar_t tempStackTopOper[] = { stack.top(), L'\0' };
					rpnList.push_back(tempStackTopOper);
					stack.pop();
				}

				stack.push(token[0]);
			}
			else if (wcscmp(token, L"(") == 0)
			{
				if (expected != OPERAND)
				{
					processTokenError({
						L"Ожидался операнд ",
						tokensList,
						tokenIter,
						6
						});
				}
				expected = OPERAND;

				stack.push(token[0]);
			}
			else if (wcscmp(token, L")") == 0)
			{
				if (expected != OPERATOR)
				{
					processTokenError({
						L"Ожидался оператор ",
						tokensList,
						tokenIter,
						6
						});
				}
				expected = OPERATOR;

				while (!stack.empty() && stack.top() != L'(')
				{
					const wchar_t tempStackTopOper[] = { stack.top(), L'\0' };
					rpnList.push_back(tempStackTopOper);
					stack.pop();
				}

				if (stack.empty())
				{
					processTokenError({
						L"Лишняя закрывающая скобка ",
						tokensList,
						tokenIter,
						6
						});
				}

				stack.pop();
			}
			else
			{
				processTokenError({
					L"Неизвестный символ ",
					tokensList,
					tokenIter,
					6
					});
			}

			prevToken = token;
			++tokenIter;
		}

		if (expected != OPERATOR && !tokensList.empty())
		{
			throw shuntingException(L"Выражение не закончено.");
		}

		while (!stack.empty())
		{
			if (stack.top() == L'(')
			{
				while (!stack.empty()) stack.pop();
				throw shuntingException(L"Пропущена закрывающая скобка.");
			}

			const wchar_t tempStackTopOper[] = { stack.top(), L'\0' };
			rpnList.push_back(tempStackTopOper);
			stack.pop();
		}

		return rpnList;
	}

	#define isZero(val) val > -1e-12 && val < 1e-12

	double calculateRpn() { return calculateRpn(rpnList); }
	
	// Throws on empty list, unknown token, undefined variable, division by zero, and infinite result.
	double calculateRpn(const pcwList &rpn)
	{
		if (rpn.empty())
		{
			throw shuntingException(L"Список RPN пуст.");
		}

		Stack<double> calcStack;

		// Trowing without trace because cannot trace on reverce polish notation
		for (const auto &token : rpn)
		{
			const auto* operatorStruct = internalOperatorHandler::get(token);

			if (operatorStruct)                          // Operator
			{
				if (operatorStruct->sign == L'~')
				{
					if (calcStack.empty())               // Unary minus. Needs one operand
					{
						throw shuntingException(L"Недостаточно операндов для унарного минуса.");
					}

					double valToPush = calcStack.top();
					calcStack.pop();
					calcStack.push(-valToPush);
				}
				else
				{
					if (calcStack.size() < 2)            // Binary operator. Needs two operands
					{
						throw shuntingException(L"Недостаточно операндов для оператора.");
					}

					double rightOperand = calcStack.top();
					calcStack.pop();
					double leftOperand = calcStack.top();
					calcStack.pop();

					double result{};
					switch (operatorStruct->sign)
					{
					case L'+':
						result = leftOperand + rightOperand;
						break;

					case L'-':
						result = leftOperand - rightOperand;
						break;

					case L'*':
						result = leftOperand * rightOperand;
						break;

					case L'/':
						if (isZero(rightOperand))
						{
							throw shuntingException(L"Деление на ноль.");
						}
						result = leftOperand / rightOperand;
						break;

					case L'^':
						result = std::pow(leftOperand, rightOperand);
						break;

					default:
						throw shuntingException(L"Неизвестный оператор.");
					}

					if (std::isinf(result))
					{
						throw shuntingException(L"Результат операции бесконечность.");
					}

					calcStack.push(result);
				}
			}
			else if (iswdigit(token[0]))                 // Number
			{
				calcStack.push(UTL::GetNumber<double>(token));
			}
			else if (iswalpha(token[0]))                 // Variable
			{
				const internalVariables* var = getVariable(token);
				if (!var)
				{
					throw shuntingException(L"Неизвестная переменная " + std::wstring(token));
				}
				calcStack.push(var->value);
			}
			else                                         // Undefined
			{
				throw shuntingException(L"Неизвестный токен " + std::wstring(token));
			}
		}

		if (calcStack.size() != 1)
		{
			throw shuntingException(L"Некорректное выражение. Лишние операнды.");
		}

		return calcStack.top();
	}

	#undef isZero
};
