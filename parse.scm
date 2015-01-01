#!/usr/bin/env guile
!#

; compile takes an expression and an environment and returns the BLC
(define (lambda? s)
  (and (symbol? s) (string-prefix? "\\" (symbol->string s))))

(define (lambda->symbol l)
  (string->symbol (substring (symbol->string l) 1)))

(define (display-index i)
  (cond
    ((zero? i) (display "0"))
    (else (display "1") (display-index (- i 1)))))

;(define (compile e env) (lambda? (car e)))
(define (compile e env) 
  ;(newline)
  ;(display "e  : ")
  ;(display e)
  ;(newline)
  ;(display "env: ")
  ;(display env)
  ;(newline)
  (cond
    ((null? e) #f)
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
  (let ((k (- (length e) 1)))
    (display "01") 
    (compile (list-head e k) env) 
    (compile (list-tail e k) env)))

(define (compile-var v env)
  (display-index (- (length env) (cdr (assq v env)))))

(define (read-all)
  (let ((expr (read)))
    (cond
      ((eof-object? expr) '())
      (else (cons expr (read-all))))))

; read and parse stdin, then compile
(compile (read-all) '())
