#!/usr/bin/env guile
!#

(define (lambda? s)
  (and (symbol? s) (char=? #\\ (car (string->list (symbol->string s))))))

(define (lambda->symbol l)
  (string->symbol (substring (symbol->string l) 1)))

(define (display-index i)
  (cond
    ((zero? i) (display "0"))
    (else (display "1") (display-index (- i 1)))))

; compile takes an expression and an environment and output the BLC to stdout
(define (compile e env) 
  ;(newline)
  ;(display "e  : ")
  ;(display e)
  ;(newline)
  ;(display "env: ")
  ;(display env)
  ;(newline)
  (cond
    ((null? e) #t)
    ((lambda? (car e)) 
       (compile-lambda e env))
    ((> (length e) 1)
       (compile-app e env))
    ((symbol? (car e)) 
       (compile-var (car e) env))
    (else
       (compile (car e) env))))

(define (compile-lambda e env)
  (display "00") 
  (compile (cdr e) (cons (cons (lambda->symbol (car e)) (length env)) env)))

(define (compile-app e env)
  (display "01")
  (compile (reverse (cdr (reverse e))) env)
  (compile (list (car (reverse e))) env))

(define (compile-var v env)
  (display-index (- (length env) (cdr (assq v env)))))

(define (read-all)
  (let ((expr (read)))
    (cond
      ((eof-object? expr) '())
      (else (cons expr (read-all))))))

; read stdin, then compile
(compile (read-all) '())
