﻿//ОПРЕДЕЛЕНИЯ И БИБЛИОТЕКИ 
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define int long long
#define bool int
#define true 1
#define false 0
#define SIZE 1024
#define STRING_SIZE 100
#define NumberOfOperators 5
#define COMPLEX_PLUS '$'

const char REPLACERO[2] = {1, 0};
const char REPLACERC[2] = {2, 0};
const char MASKREPLACE[3] = {1, '(', 0};

const long double eps = 1e-4;

typedef struct InputVariable
{
    char Name[STRING_SIZE], Value[STRING_SIZE];
} Variable;

//ФУНКЦИИ
int getOperatorPriority(const char c)
{
    switch (c)
    {
        case '^':
        case '!':
            return 4;
        case '*':
        case '/':
            return 3;
        case '+':
        case '-':
            return 2;
        case '=':
            return 1;
    }
    return 0;
}

char *replaceWord(const char *s, const char *oldW,
                  const char *newW)
{
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);

    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++)
    {
        if (strstr(&s[i], oldW) == &s[i])
        {
            cnt++;

            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }

    // Making new string of enough length
    result = (char *) malloc(i + cnt * (newWlen - oldWlen) + 1);

    i = 0;
    while (*s)
    {
        // compare the substring with the result
        if (strstr(s, oldW) == s)
        {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        }
        else
            result[i++] = *s++;
    }

    result[i] = '\0';
    return result;
}

bool isOperator(char curChar)
{
    char Operators[NumberOfOperators] = {'+', '-', '*', '/', '^'};
    for (int i = 0; i < NumberOfOperators; i++)
    {
        if (curChar == Operators[i]) return true;
    }
    return false;
}

bool isLetter(char curChar)
{
    if ((curChar >= 'a' && curChar <= 'z') || (curChar >= 'A' && curChar <= 'Z')) return true;
    else return false;
}

bool isNumber(char curChar)
{
    if (curChar >= '0' && curChar <= '9') return true;
    else return false;
}

int countVariables(char *expression)
{
    int j, cnt = 0, ptv = 0, ptn = 0;
    char variables[STRING_SIZE][STRING_SIZE] = {0}, name[STRING_SIZE] = {0};
    for (int i = 0; i < strlen(expression);)
    {
        bool f = false;
        char cur = expression[i];
        if (isLetter(cur))
        {
            for (j = i; j < strlen(expression); j++)
            {
                if (expression[j] == '(')
                {
                    f = true;
                    break;
                }
                if (!isLetter(expression[j])) break;
                name[ptn++] = expression[j];
            }
            name[ptn] = 0;
            if (!f)
            {
                bool flag = false;
                for (int j = 0; j < ptv; j++)
                {
                    if (strcmp(variables[j], name) == 0) flag = true;
                }
                if (!flag)
                {
                    cnt++;
                    strcpy(variables[ptv], name);
                    ptv++;
                }

            }
            ptn = 0;
            i += (j - i);
        }
        else i++;
    }
    return cnt;
}

char *findUnaryMinus(char *inputStr, char *output)
{
    for (int i = 0; i < strlen(inputStr); i++)
    {
        if (inputStr[i] == '-' &&
            (i == 0 || (i > 0 && (inputStr[i - 1] == '(' || isOperator(inputStr[i - 1])))))
            inputStr[i] = '!';
    }
    strcpy(output, inputStr);
    return output;
}

char *replaceComplexPlus(char *input, char complexI)
{
    char res[SIZE] = {0};
    strcpy(res, input);
    for (int i = 0; i < strlen(res); i++)
    {
        if (res[i] == '+')
        {
            int pos = i;
            for (int j = i + 1; isNumber(res[j]) || res[j] == '.'; j++)
            {
                if (res[j + 1] == complexI)
                {
                    res[pos] = COMPLEX_PLUS;
                    res[j + 1] = ' ';
                    i = j + 1;
                    break;
                }
            }
        }
    }
    return res;
}

char *makePostfixForm(char *inputStr, char *output)
{
    bool flag = false;
    char *outputStr = (char *) calloc(SIZE, SIZE);
    char *operatorStack = (char *) calloc(SIZE, SIZE);
    int StackPointer = 0, StringPointer = 0;
    for (int i = 0; i < strlen(inputStr); i++)
    {
        char cur = inputStr[i];
        if (isNumber(cur) || (cur == '.' && isNumber(inputStr[i - 1])) || cur == '!')
        {
            if (flag) outputStr[StringPointer] = cur, StringPointer++;
            else
            {
                outputStr[StringPointer] = ' ';
                StringPointer++;
                outputStr[StringPointer] = cur;
                StringPointer++;
                flag = true;
            }
        }
        else if (cur==',')
        {
            outputStr[StringPointer] = ' ';
            StringPointer++;
        }
        else if (isOperator(cur))
        {
            flag = false;
            while (StackPointer)
            {
                char temp = operatorStack[StackPointer - 1];
                if (isOperator(temp) && (getOperatorPriority(cur) <= getOperatorPriority(temp)))
                {
                    outputStr[StringPointer] = ' ';
                    StringPointer++;
                    outputStr[StringPointer] = temp;
                    StringPointer++;
                    StackPointer--;
                }
                else break;
            }
            operatorStack[StackPointer] = cur;
            StackPointer++;
        }
        else if (cur == '(')
        {
            flag = false;
            operatorStack[StackPointer] = cur;
            StackPointer++;
        }
        else if (cur == ')')
        {
            flag = false;
            bool f = false;
            while (StackPointer)
            {
                char temp = operatorStack[StackPointer - 1];
                if (temp == '(')
                {
                    f = true;
                    break;
                }
                else
                {
                    outputStr[StringPointer] = ' ';
                    StringPointer++;
                    outputStr[StringPointer] = temp;
                    StringPointer++;
                    StackPointer--;
                }
            }
            StackPointer--;
            if (isLetter(inputStr[i + 1]))
            {
                outputStr[StringPointer] = ' ';
                StringPointer++;
                outputStr[StringPointer] = inputStr[i + 1];
                StringPointer++;
            }
        }
        else
        {
            if (isLetter(cur))
            {
                if (i == 0 || (i > 0 && inputStr[i - 1] != ')'))
                {
                    outputStr[StringPointer] = ' ';
                    StringPointer++;
                    outputStr[StringPointer] = cur;
                    StringPointer++;
                    flag = true;
                }
                //Тут прописать случай если символы являются не названием переменной, а функцией
            }
        }
    }
    while (StackPointer)
    {
        outputStr[StringPointer] = ' ';
        StringPointer++;
        outputStr[StringPointer] = operatorStack[StackPointer - 1];
        StringPointer++;
        StackPointer--;
    }
    strcpy(output, outputStr);
    free(outputStr);
    free(operatorStack);
    return output;
}

double calculatePolish(char inputStr[])
{
    double *stack = (double *) malloc(SIZE);
    int sp = 0;
    for (int i = 0; i < strlen(inputStr); i++)
    {
        char c = inputStr[i];
        double x;
        char number[128] = {0};
        switch (c)
        {
            case ' ':
            case '\n':
                break;
            case '+':
                stack[sp - 2] = stack[sp - 2] + stack[sp - 1];
                sp--;
                break;
            case '-':
                stack[sp - 2] = stack[sp - 2] - stack[sp - 1];
                sp--;
                break;
            case '*':
                stack[sp - 2] = stack[sp - 1] * stack[sp - 2];
                sp--;
                break;
            case '/':
                if (stack[sp - 1] == 0)
                {
                    printf("ERROR");
                    exit(0);
                }
                stack[sp - 2] = stack[sp - 2] / stack[sp - 1];
                sp--;
                break;
            case 'p':
            case '^':
                stack[sp - 2] = pow(stack[sp - 2], stack[sp - 1]);
                sp--;
                break;
            case 's':
                stack[sp - 1] = sin(stack[sp - 1]);
                break;
            case 'c':
                stack[sp - 1] = cos(stack[sp - 1]);
                break;
            case 'n':
                stack[sp - 1] = log(stack[sp - 1]);
                break;
            case 'g':
                stack[sp - 1] = log10(stack[sp - 1]);
                break;
            case 'a':
                stack[sp - 1] = fabs(stack[sp - 1]);
                break;
            case 't':
                stack[sp - 1] = tan(stack[sp - 1]);
                break;
            case 'q':
                stack[sp - 1] = sqrt(stack[sp - 1]);
                break;
            case 'e':
                stack[sp - 1] = exp(stack[sp - 1]);
                break;
            case 'l':
                stack[sp-2] = ((double)log(stack[sp-1]))/((double)log(stack[sp-2]));
                sp--;
                break;
            default:
                for (int j = i; isNumber(inputStr[j]) || (inputStr[j] == '.' && isNumber(inputStr[j - 1]) ||
                                                          (inputStr[j] == '!' && isNumber(inputStr[j + 1]))); j++)
                {
                    if (inputStr[j] == '!')
                        number[j - i] = '-';
                    else
                        number[j - i] = inputStr[j];
                }
                i += strlen(number);
                x = atof(number);
                stack[sp] = x;
                sp++;
        }
    }
    double result = stack[sp - 1];
    return result;
}

char *deleteSpaces(char *inputStr, char *output)
{
    char withoutSpaces[SIZE] = {0};
    int pt = 0;
    for (int i = 0; i < strlen(inputStr); i++)
    {
        if (inputStr[i] != ' ')
        {
            withoutSpaces[pt] = inputStr[i];
            pt++;
        }
    }
    strcpy(output, withoutSpaces);
    return output;
}

int replaceBrackets(char *inputStr[], int posfrom)
{
    char *arr = *inputStr;
    char stack[SIZE] = {0};
    int sp = 0;
    for (int i = posfrom; i < strlen(arr); i++)
    {

        if (arr[i] == REPLACERO[0] && arr[i + 1] == '(')
        {
            i = replaceBrackets(&arr, i + 1);
        }
        if (arr[i] == '(') stack[sp++] = 1;
        else if (arr[i] == ')')
        {
            stack[sp--] = 0;
            if (sp == 0)
            {
                arr[i] = REPLACERC[0];
                strcpy(*inputStr, arr);
                return i;
            }
        }
    }
}

char *replaceFuncToBrackets(char inputStr[], char name[], char toBeginning[], char toEnd[])
{
    char *result = (char *) calloc(SIZE, SIZE);
    char stack[SIZE] = {0};
    int sp = 0;
    bool found = false;
    strcpy(result, replaceWord(inputStr, name, REPLACERO));
    for (int i = 0; i < strlen(result); i++)
    {
        if (result[i] == REPLACERO[0] && result[i + 1] == '(')
        {
            i = replaceBrackets(&result, i + 1);
        }
    }

    strcpy(result, replaceWord(result, MASKREPLACE, toBeginning));
    strcpy(result, replaceWord(result, REPLACERC, toEnd));
    //printf("%s\n", result);
    return result;
}

char *makeSuitableForm(char *expression)
{
    strcpy(expression, replaceWord(expression, "PI", "3.1415926"));
    strcpy(expression, replaceWord(expression, "E", "2.71828"));
    strcpy(expression, deleteSpaces(expression, expression));
//    strcpy(expression, replaceComplexPlus(expression, 'J'));
    strcpy(expression, replaceWord(expression, "sin", "с"));
    strcpy(expression, replaceWord(expression, "cos", "к"));
    strcpy(expression, replaceWord(expression, "tg", "т"));
    strcpy(expression, replaceWord(expression, "log", "л"));
    strcpy(expression, replaceWord(expression, "ln", "н"));
    strcpy(expression, replaceWord(expression, "lg", "г"));
    strcpy(expression, replaceWord(expression, "sqrt", "р"));
    strcpy(expression, replaceWord(expression, "abs", "б"));
    strcpy(expression, replaceWord(expression, "exp", "е"));
    strcpy(expression, replaceWord(expression, "pow", "п"));
    return expression;
}

double calculateExpression(char *expression)
{
    char result[SIZE] = {0}, temp1[SIZE] = {0}, temp2[SIZE] = {0};
    strcpy(temp1, deleteSpaces(expression, temp1));
    strcpy(temp2, findUnaryMinus(temp1, temp2));
    strcpy(temp2, makeSuitableForm(temp2));
    Variable VariableData[STRING_SIZE] = {0};
    int NumberOfVariables = countVariables(expression);
    for (int i = 0; i < NumberOfVariables; i++)
    {
        char str[STRING_SIZE] = {0};
        fgets(str, sizeof(str), stdin);
        char *rest = strchr(str, (int) '=');
        rest[strlen(rest) - 1] = 0;
        sscanf(str, "%s", VariableData[i].Name);
        strcpy(VariableData[i].Value, rest);
    }
    while (countVariables(expression))
    {
        strcpy(expression, replaceWord(expression, "PI", "3.1415926"));
        strcpy(expression, replaceWord(expression, "E", "2.71828"));
        int NumberOfVariables = countVariables(expression);
        for (int i = 0; i < NumberOfVariables; i++)
        {
            strcpy(expression, replaceWord(expression, VariableData[i].Name, VariableData[i].Value));
        }
    }
    strcpy(temp2, makeSuitableForm(temp2));
    strcpy(temp2, replaceFuncToBrackets(temp2, "с", "(", ")s"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "к", "(", ")c"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "т", "(", ")t"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "л", "(", ")l"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "н", "(", ")n"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "г", "(", ")g"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "р", "(", ")q"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "б", "(", ")a"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "е", "(", ")e"));
    strcpy(temp2, replaceFuncToBrackets(temp2, "п", "(", ")p"));
    makePostfixForm(temp2, result);
    printf("DEBUG: %s\n", result);
    return calculatePolish(result);
}

void check()
{
    FILE *Tests, *Answers;
    Tests = fopen("Tests.txt", "r");
    Answers = fopen("TestAnswers.txt", "r");
    char temp[STRING_SIZE] = {0};
    fgets(temp, STRING_SIZE, Tests);
    int NumberOfTests;
    sscanf(temp, "%d", &NumberOfTests);
    for (int test_case = 1; test_case <= NumberOfTests; test_case++)
    {
        char result[SIZE] = {0}, temp1[SIZE] = {0}, temp2[SIZE] = {0}, expression[SIZE] = {0};;
        fgets(expression, sizeof(expression), Tests);
        strcpy(expression, makeSuitableForm(expression));
        strcpy(temp1, deleteSpaces(expression, temp1));
        strcpy(temp2, findUnaryMinus(temp1, temp2));
        strcpy(temp2, makeSuitableForm(temp2));
        Variable VariableData[STRING_SIZE] = {0};
        int NumberOfVariables = countVariables(expression);
        for (int i = 0; i < NumberOfVariables; i++)
        {
            char str[STRING_SIZE] = {0};
            fgets(str, sizeof(str), Tests);
            char *rest = strchr(str, (int) '=');
            rest[strlen(rest) - 1] = 0;
            sscanf(str, "%s", VariableData[i].Name);
            strcpy(VariableData[i].Value, rest);
        }
        while (countVariables(expression))
        {
            strcpy(expression, replaceWord(expression, "PI", "3.1415926"));
            strcpy(expression, replaceWord(expression, "E", "2.71828"));
            int NumberOfVariables = countVariables(expression);
            for (int i = 0; i < NumberOfVariables; i++)
            {
                strcpy(expression, replaceWord(expression, VariableData[i].Name, VariableData[i].Value));
            }
        }
        strcpy(temp2, makeSuitableForm(temp2));
        strcpy(temp2, replaceFuncToBrackets(temp2, "с", "(", ")s"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "к", "(", ")c"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "т", "(", ")t"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "л", "(", ")l"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "н", "(", ")n"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "г", "(", ")g"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "р", "(", ")q"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "б", "(", ")a"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "е", "(", ")e"));
        strcpy(temp2, replaceFuncToBrackets(temp2, "п", "(", ")p"));
        makePostfixForm(temp2, result);
        double AnswerForCurrentTest;
        char StringWithAnswer[STRING_SIZE] = {0};
        fgets(StringWithAnswer, sizeof(StringWithAnswer), Answers);
        sscanf(StringWithAnswer, "%lf", &AnswerForCurrentTest);
        if (abs(AnswerForCurrentTest - calculatePolish(result)) > eps)
        {
            printf("Wrong answer on test_case #%d: (%lf) instead of (%lf)\n", test_case, calculatePolish(result),
                   AnswerForCurrentTest);
        }
    }
}

signed main()
{

    char Expression[SIZE];
    fgets(Expression, sizeof(Expression), stdin);
    double Result = calculateExpression(Expression);
    printf("%lf", Result);
//    check();
    return 0;
}
