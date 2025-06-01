package org.example;

// 1. 문자열 순회
// 2. 스택 맨위가 ( 이고, 문자가 ) 이면 스택 pop
// 3. 그렇지 않으면 스택 push
// 4. 정답: 스택의 크기

import java.util.*;
import java.io.*;

public class Main {
    public static void main(String[] args) throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        String str = br.readLine();

        Stack<Character> stack = new Stack<>();
        // 1. 문자열 순회
        for(int i = 0; i < str.length(); i++) {

            // 2. 스택 맨위가 ( 이고, 문자가 ) 이면 스택 pop
            if (!stack.isEmpty() && stack.peek() == '(' && str.charAt(i) == ')') {
                stack.pop();
            }

            // 3. 그렇지 않으면 스택 push
            else {
                stack.push(str.charAt(i));
            }
        }

        // 4. 정답: 스택의 크기
        System.out.println(stack.size());
    }
}
