#include <iostream>
#include <sstream>

#include "types.h"
#include "print.h"
#include "lists.h"
#include "parse.h"

using namespace std;

template<typename T> void compare_stringstream(const char* expected, T) {
    std::stringstream s;
    s << print<T>();
    if (s.str() != expected) {
        std::cout << "Test failed: Expected " << expected << " but got " << s.str() << std::endl;
        //std::cout << "typeid: " << typeid(T).name() << std::endl;
    }
}

int main()
{
    // Store to a variable to ensure constexprness
#define TEST(expected, lit) \
    { \
        constexpr auto expr = lit; \
        std::cout << "parsed: " << print(expr) << std::endl; \
        compare_stringstream(expected, expr); \
    }

    TEST("nil", "()"_lisp);
    TEST("(quote nil)", "'()"_lisp);
    TEST("(nil nil)", "(() ())"_lisp);
    TEST("(nil nil nil)", "(() () ())"_lisp);
    // numbers
    TEST("12345", "12345"_lisp);
    TEST("-48", "-48"_lisp); // TODO Check that we get a number and not a symbol, they'll print the same...
    TEST("(1 2 3 4 5)", "(1 2 3 4 5)"_lisp);
    // symbols
    TEST("foo", "foo"_lisp);
    TEST("(foo bar)", "(foo bar)"_lisp);
    TEST("(foo-bar-123+-/*?<>!= baz)", "(foo-bar-123+-/*?<>!= baz)"_lisp);
    //whitespace
    TEST("nil", " ()"_lisp);
    TEST("nil", " ( )"_lisp);
    TEST("nil", " \t\n( )"_lisp);
    TEST("nil", " \t\n( \t\n)"_lisp);
    TEST("symbol-after-newline", R"(
    symbol-after-newline )"_lisp);
    TEST("(list-after-newline)", R"(
    (list-after-newline) )"_lisp);
    TEST("(foo bar)", R"( (foo
    bar) )"_lisp);
    // comments
    TEST("nil", ";\n()"_lisp);
    TEST("nil", "; comment\n()"_lisp);
    TEST("nil", "(; comment\n)"_lisp);

    // strings
    TEST("string", "\"string\""_lisp); // TODO Change string printing to include quotes

#if 1
    constexpr auto sicp_mceval1 = R"foo((progn(define apply-in-underlying-scheme apply)(define(eval exp env)(cond((self-evaluating? exp)exp)((variable? exp)(lookup-variable-value exp env))((quoted? exp)(text-of-quotation exp))((assignment? exp)(eval-assignment exp env))((definition? exp)(eval-definition exp env))((if? exp)(eval-if exp env))((lambda? exp)(make-procedure(lambda-parameters exp)(lambda-body exp)env))((begin? exp)(eval-sequence(begin-actions exp)env))((cond? exp)(eval(cond->if exp)env))((application? exp)(apply(eval(operator exp)env)(list-of-values(operands exp)env)))(else(error "Unknown expression type -- EVAL" exp))))(define(apply procedure arguments)(cond((primitive-procedure? procedure)(apply-primitive-procedure procedure arguments))((compound-procedure? procedure)(eval-sequence(procedure-body procedure)(extend-environment(procedure-parameters procedure)arguments(procedure-environment procedure))))(else(error "Unknown procedure type -- APPLY" procedure))))(define(list-of-values exps env)(if(no-operands? exps)'nil(cons(eval(first-operand exps)env)(list-of-values(rest-operands exps)env))))(define(eval-if exp env)(if(true? (eval(if-predicate exp)env))(eval(if-consequent exp)env)(eval(if-alternative exp)env)))(define(eval-sequence exps env)(cond((last-exp? exps)(eval(first-exp exps)env))(else(eval(first-exp exps)env)(eval-sequence(rest-exps exps)env))))(define(eval-assignment exp env)(let((var(assignment-variable exp))(val(eval(assignment-value exp)env)))(cond((pair? var)(cond((eq? 'car(car var))(set-car! (eval(cadr var)env)val))((eq? 'cdr(car var))(set-cdr! (eval(cadr var)env)val))))(else(set-variable-value! var val env)))'ok))(define(eval-definition exp env)(define-variable! (definition-variable exp)(eval(definition-value exp)env)env)'ok)(define(self-evaluating? exp)(cond((number? exp)true)((string? exp)true)(else false)))(define(quoted? exp)(tagged-list? exp 'quote))(define(text-of-quotation exp)(cadr exp))(define(tagged-list? exp tag)(if(pair? exp)(eq? (car exp)tag)false))(define(variable? exp)(symbol? exp))(define(assignment? exp)(tagged-list? exp 'set!))(define(assignment-variable exp)(cadr exp))(define(assignment-value exp)(caddr exp))(define(definition? exp)(tagged-list? exp 'define))(define(definition-variable exp)(if(symbol? (cadr exp))(cadr exp)(caadr exp)))(define(definition-value exp)(if(symbol? (cadr exp))(caddr exp)(make-lambda(cdadr exp)(cddr exp))))(define(lambda? exp)(tagged-list? exp 'lambda))(define(lambda-parameters exp)(cadr exp))(define(lambda-body exp)(cddr exp))(define(make-lambda parameters body)(cons 'lambda(cons parameters body)))(define(if? exp)(tagged-list? exp 'if))(define(if-predicate exp)(cadr exp))(define(if-consequent exp)(caddr exp))(define(if-alternative exp)(if(not(null? (cdddr exp)))(cadddr exp)'false))(define(make-if predicate consequent alternative)(list 'if predicate consequent alternative))(define(begin? exp)(tagged-list? exp 'begin))(define(begin-actions exp)(cdr exp))(define(last-exp? seq)(null? (cdr seq)))(define(first-exp seq)(car seq))(define(rest-exps seq)(cdr seq))(define(sequence->exp seq)(cond((null? seq)seq)((last-exp? seq)(first-exp seq))(else(make-begin seq))))(define(make-begin seq)(cons 'begin seq))(define(application? exp)(pair? exp))(define(operator exp)(car exp))(define(operands exp)(cdr exp))(define(no-operands? ops)(null? ops))(define(first-operand ops)(car ops))(define(rest-operands ops)(cdr ops))(define(cond? exp)(tagged-list? exp 'cond))(define(cond-clauses exp)(cdr exp))(define(cond-else-clause? clause)(eq? (cond-predicate clause)'else))(define(cond-predicate clause)(car clause))(define(cond-actions clause)(cdr clause))(define(cond->if exp)(expand-clauses(cond-clauses exp)))(define(expand-clauses clauses)(if(null? clauses)'false(let((first(car clauses))(rest(cdr clauses)))(if(cond-else-clause? first)(if(null? rest)(sequence->exp(cond-actions first))(error "ELSE clause isn't last -- COND->IF" clauses))(make-if(cond-predicate first)(sequence->exp(cond-actions first))(expand-clauses rest))))))(define(true? x)(not(eq? x false)))(define(false? x)(eq? x false))(define(make-procedure parameters body env)(list 'procedure parameters body env))(define(compound-procedure? p)(tagged-list? p 'procedure))(define(procedure-parameters p)(cadr p))(define(procedure-body p)(caddr p))(define(procedure-environment p)(cadddr p))(define(enclosing-environment env)(cdr env))(define(first-frame env)(car env))(define the-empty-environment 'nil)(define(make-frame variables values)(cons variables values))(define(frame-variables frame)(car frame))(define(frame-values frame)(cdr frame))(define(add-binding-to-frame! var val frame)(set-car! frame(cons var(car frame)))(set-cdr! frame(cons val(cdr frame))))(define(extend-environment vars vals base-env)(if(= (length vars)(length vals))(cons(make-frame vars vals)base-env)(if(< (length vars)(length vals))(error "Too many arguments supplied" vars vals)(error "Too few arguments supplied" vars vals))))(define(lookup-variable-value var env)(define(env-loop env)(define(scan vars vals)(cond((null? vars)(env-loop(enclosing-environment env)))((eq? var(car vars))(car vals))(else(scan(cdr vars)(cdr vals)))))(if(eq? env the-empty-environment)(error "Unbound variable" var)(let((frame(first-frame env)))(scan(frame-variables frame)(frame-values frame)))))(env-loop env))(define(set-variable-value! var val env)(define(env-loop env)(define(scan vars vals)(cond((null? vars)(env-loop(enclosing-environment env)))((eq? var(car vars))(set-car! vals val))(else(scan(cdr vars)(cdr vals)))))(if(eq? env the-empty-environment)(error "Unbound variable -- SET!" var)(let((frame(first-frame env)))(scan(frame-variables frame)(frame-values frame)))))(env-loop env))(define(define-variable! var val env)(let((frame(first-frame env)))(define(scan vars vals)(cond((null? vars)(add-binding-to-frame! var val frame))((eq? var(car vars))(set-car! vals val))(else(scan(cdr vars)(cdr vals)))))(scan(frame-variables frame)(frame-values frame))))(define(setup-environment)(let((initial-env(extend-environment(primitive-procedure-names)(primitive-procedure-objects)the-empty-environment)))(define-variable! 'true true initial-env)(define-variable! 'false false initial-env)initial-env))(define(primitive-procedure? proc)(tagged-list? proc 'primitive))(define(primitive-implementation proc)(cadr proc))(define primitive-procedures(list(list 'car car)(list 'cdr cdr)(list 'cons cons)(list 'null? null?)))(define(primitive-procedure-names)(map car primitive-procedures))(define(primitive-procedure-objects)(map(lambda(proc)(list 'primitive(cadr proc)))primitive-procedures))(define(apply-primitive-procedure proc args)(apply-in-underlying-scheme(primitive-implementation proc)args))(define input-prompt ";;; M-Eval input:")(define output-prompt ";;; M-Eval value:")(define(driver-loop)(prompt-for-input input-prompt)(let((input(read)))(let((output(eval input the-global-environment)))(announce-output output-prompt)(user-print output)))(driver-loop))(define(prompt-for-input string)(newline)(newline)(display string)(newline))(define(announce-output string)(newline)(display string)(newline))(define(user-print object)(if(compound-procedure? object)(display(list 'compound-procedure(procedure-parameters object)(procedure-body object)'<procedure-env>))(display object)))'METACIRCULAR-EVALUATOR-LOADED(define *buf-char* nil)(define real-getc getc)(define(lookc)(if(null? *buf-char*)(set! *buf-char* (real-getc)) *buf-char*))(define(getc)(let((c(lookc)))(set! *buf-char* nil)c))(define(ungetc c)(set! *buf-char* c))(define(read)(let((c(whitespace-getc)))(cond((= c 40)(read-list))((= c 39)(list 'quote(read)))((symbol-char1 c)(list->symbol(cons c(read-symbol))))((elem c digits)(list->number(read-number c)))((null? c)(error "eof" c))(else(error "Unexpected character" c)))))(define(read-list)(let((res(cons nil nil)))(read-rest res)(cdr res)))(define(read-end-paren)(whitespace-getc))(define(read-rest target)(let((c(whitespace-getc)))(cond((= c 46)(progn(set-cdr! target(read))(read-end-paren)))((= c 41)nil)(else(progn(ungetc c)(let((res(cons(read)nil)))(set-cdr! target res)(read-rest res)))))))(define(read-symbol)(let((c(getc)))(cond((symbol-charn c)(cons c(read-symbol)))(else(progn(ungetc c)nil)))))(define(read-number c)(let((n(lookc)))(if(elem n digits)(cons c(read-number(getc)))(list c))))(define(whitespace-getc)(let((c(getc)))(cond((or(= c 10)(= c 32))(whitespace-getc))((= c 59)(eat-comment-getc))(else c))))(define(eat-comment-getc)(if(= 10(getc))(whitespace-getc)(eat-comment-getc)))(define symbol-chars1 '(33 42 43 45 47 60 61 62 63 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122))(define symbol-charsn '(33 42 43 45 47 48 49 50 51 52 53 54 55 56 57 60 61 62 63 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122))(define digits '(48 49 50 51 52 53 54 55 56 57))(define(symbol-char1 c)(elem c symbol-chars1))(define(symbol-charn c)(if(symbol-char1 c)'t(elem c symbol-charsn)))(define(list->number cs)(define(go acc cs)(cond((null? cs)acc)(else(go(+ (* 10 acc)(car cs) -48)(cdr cs)))))(go 0 cs))(define(map fun list)(if(null? list)nil(cons(fun(car list))(map fun(cdr list)))))(define(caadr x)(car(cadr x)))(define(cadddr x)(car(cdddr x)))(define(caddr x)(car(cddr x)))(define(cadr x)(car(cdr x)))(define(cdadr x)(cdr(cadr x)))(define(cdddr x)(cdr(cddr x)))(define(cddr x)(cdr(cdr x)))(define(length list)(if(null? list)0(+ 1(length(cdr list)))))(define = eq?)(define true 't)(define false nil)(define else 't)(define(or x y)(if x 't y))(define(not x)(if x false true))(define(set-car! x y)(set! (car x)y))(define(set-cdr! x y)(set! (cdr x)y))(define(newline)(putc 10))(define(elem x list)(cond((null? list)nil)((eq? (car list)x)'t)(else(elem x(cdr list)))))(define(append a b)(cond((null? a)b)(else(cons(car a)(append(cdr a)b)))))(define(begin? exp)(or(tagged-list? exp 'begin)(tagged-list? exp 'progn)))(define(error msg exp)(display msg)(display exp)(abort))(define primitive-procedures(append primitive-procedures(list(list '+ +)(list '* *))))(define the-global-environment(setup-environment))(define(driver-loop)(prompt-for-input input-prompt)(let((input(read)))(if(eq? input 'quit)nil(progn(let((output(eval input the-global-environment)))(announce-output output-prompt)(user-print output))(driver-loop)))))(define(read-eval-print)(user-print(eval(read)the-global-environment)))(define(user-print-filter object)(if(compound-procedure? object) "<procedure>" object))(define(read-eval)(user-print-filter(eval(read)the-global-environment)))(read-eval)))foo"_lisp;

#else
    constexpr auto sicp_mceval1 = R"foo((progn

;ch4-mceval.scm

;;; Structure and Interpretation of Computer Programs by Harold Abelson and
;;; Gerald Jay Sussman is licensed under a Creative Commons
;;; Attribution-ShareAlike 3.0 Unported License.
;;;
;;; See http://creativecommons.org/licenses/by-sa/3.0/

;;;;METACIRCULAR EVALUATOR FROM CHAPTER 4 (SECTIONS 4.1.1-4.1.4) of
;;;; STRUCTURE AND INTERPRETATION OF COMPUTER PROGRAMS

;;;;Matches code in ch4.scm

;;;;This file can be loaded into Scheme as a whole.
;;;;Then you can initialize and start the evaluator by evaluating
;;;; the two commented-out lines at the end of the file (setting up the
;;;; global environment and starting the driver loop).

;;;;**WARNING: Don't load this file twice (or you'll lose the primitives
;;;;  interface, due to renamings of apply).

;;;from section 4.1.4 -- must precede def of metacircular apply
(define apply-in-underlying-scheme apply)

;;;SECTION 4.1.1

(define (eval exp env)
  (cond ((self-evaluating? exp) exp)
        ((variable? exp) (lookup-variable-value exp env))
        ((quoted? exp) (text-of-quotation exp))
        ((assignment? exp) (eval-assignment exp env))
        ((definition? exp) (eval-definition exp env))
        ((if? exp) (eval-if exp env))
        ((lambda? exp)
         (make-procedure (lambda-parameters exp)
                         (lambda-body exp)
                         env))
        ((begin? exp) 
         (eval-sequence (begin-actions exp) env))
        ((cond? exp) (eval (cond->if exp) env))
        ((application? exp)
         (apply (eval (operator exp) env)
                (list-of-values (operands exp) env)))
        (else
         (error "Unknown expression type -- EVAL" exp))))

(define (apply procedure arguments)
  (cond ((primitive-procedure? procedure)
         (apply-primitive-procedure procedure arguments))
        ((compound-procedure? procedure)
         (eval-sequence
           (procedure-body procedure)
           (extend-environment
             (procedure-parameters procedure)
             arguments
             (procedure-environment procedure))))
        (else
         (error
          "Unknown procedure type -- APPLY" procedure))))


(define (list-of-values exps env)
  (if (no-operands? exps)
      '()
      (cons (eval (first-operand exps) env)
            (list-of-values (rest-operands exps) env))))

(define (eval-if exp env)
  (if (true? (eval (if-predicate exp) env))
      (eval (if-consequent exp) env)
      (eval (if-alternative exp) env)))

(define (eval-sequence exps env)
  (cond ((last-exp? exps) (eval (first-exp exps) env))
        (else (eval (first-exp exps) env)
              (eval-sequence (rest-exps exps) env))))

(define (eval-assignment exp env)
  (let ((var (assignment-variable exp))
        (val (eval (assignment-value exp) env)))
    (cond
      ; SB: Support for (set! (car ...) ...) and (set! (cdr ...) ...)
      ((pair? var)
       (cond
         ((eq? 'car (car var))
          (set-car! (eval (cadr var) env) val))
         ((eq? 'cdr (car var))
          (set-cdr! (eval (cadr var) env) val))))
      (else
       (set-variable-value! var val env)))
    'ok))

(define (eval-definition exp env)
  (define-variable! (definition-variable exp)
                    (eval (definition-value exp) env)
                    env)
  'ok)

;;;SECTION 4.1.2

(define (self-evaluating? exp)
  (cond ((number? exp) true)
        ((string? exp) true)
        (else false)))

(define (quoted? exp)
  (tagged-list? exp 'quote))

(define (text-of-quotation exp) (cadr exp))

(define (tagged-list? exp tag)
  (if (pair? exp)
      (eq? (car exp) tag)
      false))

(define (variable? exp) (symbol? exp))

(define (assignment? exp)
  (tagged-list? exp 'set!))

(define (assignment-variable exp) (cadr exp))

(define (assignment-value exp) (caddr exp))


(define (definition? exp)
  (tagged-list? exp 'define))

(define (definition-variable exp)
  (if (symbol? (cadr exp))
      (cadr exp)
      (caadr exp)))

(define (definition-value exp)
  (if (symbol? (cadr exp))
      (caddr exp)
      (make-lambda (cdadr exp)
                   (cddr exp))))

(define (lambda? exp) (tagged-list? exp 'lambda))

(define (lambda-parameters exp) (cadr exp))
(define (lambda-body exp) (cddr exp))

(define (make-lambda parameters body)
  (cons 'lambda (cons parameters body)))


(define (if? exp) (tagged-list? exp 'if))

(define (if-predicate exp) (cadr exp))

(define (if-consequent exp) (caddr exp))

(define (if-alternative exp)
  (if (not (null? (cdddr exp)))
      (cadddr exp)
      'false))

(define (make-if predicate consequent alternative)
  (list 'if predicate consequent alternative))


(define (begin? exp) (tagged-list? exp 'begin))

(define (begin-actions exp) (cdr exp))

(define (last-exp? seq) (null? (cdr seq)))
(define (first-exp seq) (car seq))
(define (rest-exps seq) (cdr seq))

(define (sequence->exp seq)
  (cond ((null? seq) seq)
        ((last-exp? seq) (first-exp seq))
        (else (make-begin seq))))

(define (make-begin seq) (cons 'begin seq))


(define (application? exp) (pair? exp))
(define (operator exp) (car exp))
(define (operands exp) (cdr exp))

(define (no-operands? ops) (null? ops))
(define (first-operand ops) (car ops))
(define (rest-operands ops) (cdr ops))


(define (cond? exp) (tagged-list? exp 'cond))

(define (cond-clauses exp) (cdr exp))

(define (cond-else-clause? clause)
  (eq? (cond-predicate clause) 'else))

(define (cond-predicate clause) (car clause))

(define (cond-actions clause) (cdr clause))

(define (cond->if exp)
  (expand-clauses (cond-clauses exp)))

(define (expand-clauses clauses)
  (if (null? clauses)
      'false                          ; no else clause
      (let ((first (car clauses))
            (rest (cdr clauses)))
        (if (cond-else-clause? first)
            (if (null? rest)
                (sequence->exp (cond-actions first))
                (error "ELSE clause isn't last -- COND->IF"
                       clauses))
            (make-if (cond-predicate first)
                     (sequence->exp (cond-actions first))
                     (expand-clauses rest))))))

;;;SECTION 4.1.3

(define (true? x)
  (not (eq? x false)))

(define (false? x)
  (eq? x false))


(define (make-procedure parameters body env)
  (list 'procedure parameters body env))

(define (compound-procedure? p)
  (tagged-list? p 'procedure))


(define (procedure-parameters p) (cadr p))
(define (procedure-body p) (caddr p))
(define (procedure-environment p) (cadddr p))


(define (enclosing-environment env) (cdr env))

(define (first-frame env) (car env))

(define the-empty-environment '())

(define (make-frame variables values)
  (cons variables values))

(define (frame-variables frame) (car frame))
(define (frame-values frame) (cdr frame))

(define (add-binding-to-frame! var val frame)
  (set-car! frame (cons var (car frame)))
  (set-cdr! frame (cons val (cdr frame))))

(define (extend-environment vars vals base-env)
  (if (= (length vars) (length vals))
      (cons (make-frame vars vals) base-env)
      (if (< (length vars) (length vals))
          (error "Too many arguments supplied" vars vals)
          (error "Too few arguments supplied" vars vals))))

(define (lookup-variable-value var env)
  (define (env-loop env)
    (define (scan vars vals)
      (cond ((null? vars)
             (env-loop (enclosing-environment env)))
            ((eq? var (car vars))
             (car vals))
            (else (scan (cdr vars) (cdr vals)))))
    (if (eq? env the-empty-environment)
        (error "Unbound variable" var)
        (let ((frame (first-frame env)))
          (scan (frame-variables frame)
                (frame-values frame)))))
  (env-loop env))

(define (set-variable-value! var val env)
  (define (env-loop env)
    (define (scan vars vals)
      (cond ((null? vars)
             (env-loop (enclosing-environment env)))
            ((eq? var (car vars))
             (set-car! vals val))
            (else (scan (cdr vars) (cdr vals)))))
    (if (eq? env the-empty-environment)
        (error "Unbound variable -- SET!" var)
        (let ((frame (first-frame env)))
          (scan (frame-variables frame)
                (frame-values frame)))))
  (env-loop env))

(define (define-variable! var val env)
  (let ((frame (first-frame env)))
    (define (scan vars vals)
      (cond ((null? vars)
             (add-binding-to-frame! var val frame))
            ((eq? var (car vars))
             (set-car! vals val))
            (else (scan (cdr vars) (cdr vals)))))
    (scan (frame-variables frame)
          (frame-values frame))))

;;;SECTION 4.1.4

(define (setup-environment)
  (let ((initial-env
         (extend-environment (primitive-procedure-names)
                             (primitive-procedure-objects)
                             the-empty-environment)))
    (define-variable! 'true true initial-env)
    (define-variable! 'false false initial-env)
    initial-env))

;[do later] (define the-global-environment (setup-environment))

(define (primitive-procedure? proc)
  (tagged-list? proc 'primitive))

(define (primitive-implementation proc) (cadr proc))

(define primitive-procedures
  (list (list 'car car)
        (list 'cdr cdr)
        (list 'cons cons)
        (list 'null? null?)
;;      more primitives
        ))

(define (primitive-procedure-names)
  (map car
       primitive-procedures))

(define (primitive-procedure-objects)
  (map (lambda (proc) (list 'primitive (cadr proc)))
       primitive-procedures))

;[moved to start of file] (define apply-in-underlying-scheme apply)

(define (apply-primitive-procedure proc args)
  (apply-in-underlying-scheme
   (primitive-implementation proc) args))



(define input-prompt ";;; M-Eval input:")
(define output-prompt ";;; M-Eval value:")

(define (driver-loop)
  (prompt-for-input input-prompt)
  (let ((input (read)))
    (let ((output (eval input the-global-environment)))
      (announce-output output-prompt)
      (user-print output)))
  (driver-loop))

(define (prompt-for-input string)
  (newline) (newline) (display string) (newline))

(define (announce-output string)
  (newline) (display string) (newline))

(define (user-print object)
  (if (compound-procedure? object)
      (display (list 'compound-procedure
                     (procedure-parameters object)
                     (procedure-body object)
                     '<procedure-env>))
      (display object)))

;;;Following are commented out so as not to be evaluated when
;;; the file is loaded.
;;(define the-global-environment (setup-environment))
;;(driver-loop)

'METACIRCULAR-EVALUATOR-LOADED

; mceval-wrapper.scm

; vim:et:ts=2:sw=2:

(define *buf-char* nil)
(define real-getc getc)
(define (lookc)
  (if (null? *buf-char*) (set! *buf-char* (real-getc)) *buf-char*))
(define (getc)
  (let ((c (lookc)))
    (set! *buf-char* nil)
    c))
(define (ungetc c) (set! *buf-char* c))

(define (read)
  (let ((c (whitespace-getc)))
    (cond
      ((= c 40) (read-list)) ; '('
      ((= c 39) (list 'quote (read)))
      ((symbol-char1 c) (list->symbol (cons c (read-symbol))))
      ((elem c digits) (list->number (read-number c)))
      ((null? c) (error "eof" c))
      (else (error "Unexpected character" c)))))
(define (read-list)
  ;(display "read-list")
  (let ((res (cons nil nil)))
    (read-rest res)
    ;(display (cdr res))
    (cdr res)))
(define (read-end-paren)
  ;(display "read-end-paren")
  (whitespace-getc))
(define (read-rest target)
  ;(display "read-rest")
  (let ((c (whitespace-getc)))
    (cond
      ((= c 46) (progn (set-cdr! target (read)) (read-end-paren)))
      ((= c 41) nil) ;(display "read-rest found ')'"))
      (else
        (progn
          (ungetc c)
          (let ((res (cons (read) nil)))
              ;(display (cons "read-rest has read a car" (car res)))
              (set-cdr! target res)
              (read-rest res)))))))
(define (read-symbol)
  ;(display "read-symbol")
  (let ((c (getc)))
    (cond
      ((symbol-charn c) (cons c (read-symbol)))
      (else (progn (ungetc c) nil)))))
(define (read-number c)
  (let ((n (lookc)))
    (if (elem n digits) (cons c (read-number (getc))) (list c))))

(define (whitespace-getc)
  (let ((c (getc)))
    (cond
      ((or (= c 10) (= c 32)) (whitespace-getc))
      ((= c 59) (eat-comment-getc))
      (else c))))
(define (eat-comment-getc)
  ;(display "eat-comment-getc")
  (if (= 10 (getc)) (whitespace-getc) (eat-comment-getc)))

; Generated by this snippet of Python, given the definition of isSymChar in
; runlisp.py:
; ' '.join(map(str,[c for c in range(256) if isSymChar(chr(c), True)]))
(define symbol-chars1 '(33 42 43 45 47 60 61 62 63 65 66 67 68 69 70 71 72 73
                        74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 97
                        98 99 100 101 102 103 104 105 106 107 108 109 110 111
                        112 113 114 115 116 117 118 119 120 121 122))
; ' '.join(map(str,[c for c in range(256) if isSymChar(chr(c), False)]))
(define symbol-charsn '(33 42 43 45 47 48 49 50 51 52 53 54 55 56 57 60 61 62
                        63 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81
                        82 83 84 85 86 87 88 89 90 97 98 99 100 101 102 103 104
                        105 106 107 108 109 110 111 112 113 114 115 116 117 118
                        119 120 121 122))
(define digits '(48 49 50 51 52 53 54 55 56 57))

(define (symbol-char1 c) (elem c symbol-chars1))
(define (symbol-charn c) (if (symbol-char1 c) 't (elem c symbol-charsn)))

(define (list->number cs)
  (define (go acc cs)
    (cond
      ((null? cs) acc)
      (else       (go (+ (* 10 acc) (car cs) -48) (cdr cs)))))
  (go 0 cs))

(define (map fun list)
  (if (null? list) nil (cons (fun (car list)) (map fun (cdr list)))))
(define (caadr x) (car (cadr x)))
(define (cadddr x) (car (cdddr x)))
(define (caddr x) (car (cddr x)))
(define (cadr x) (car (cdr x)))
(define (cdadr x) (cdr (cadr x)))
(define (cdddr x) (cdr (cddr x)))
(define (cddr x) (cdr (cdr x)))
(define (length list)
  (if (null? list) 0 (+ 1 (length (cdr list)))))
(define = eq?)
(define true 't)
(define false nil)
(define else 't)
(define (or x y) (if x 't y))
(define (not x) (if x false true))
(define (set-car! x y) (set! (car x) y))
(define (set-cdr! x y) (set! (cdr x) y))
(define (newline) (putc 10))
(define (elem x list)
  (cond
    ((null? list) nil)
    ((eq? (car list) x) 't)
    (else (elem x (cdr list)))))

(define (append a b)
  (cond
    ((null? a) b)
    (else      (cons (car a) (append (cdr a) b)))))

; Ought to change the other interpreters and the test cases to use begin
; instead.
(define (begin? exp)
  (or (tagged-list? exp 'begin) (tagged-list? exp 'progn)))

(define (error msg exp) (display msg) (display exp) (abort))

(define primitive-procedures
  (append
    primitive-procedures
    (list (list '+ +)
          (list '* *)
          ;;      more primitives
          )))

; from ch4-mceval.scm
(define the-global-environment (setup-environment))
; Copy that checks for 'quit'
(define (driver-loop)
  (prompt-for-input input-prompt)
  (let ((input (read)))
    (if (eq? input 'quit) nil
      (progn
        (let ((output (eval input the-global-environment)))
          (announce-output output-prompt)
          (user-print output))
        (driver-loop)))))

(define (read-eval-print)
  (user-print (eval (read) the-global-environment)))

(define (user-print-filter object)
  (if (compound-procedure? object) "<procedure>" object))
(define (read-eval)
  (user-print-filter (eval (read) the-global-environment)))

(read-eval)
))foo"_lisp;
#endif
    //std::cout << print(sicp_mceval1) << std::endl;
}
