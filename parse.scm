#!/usr/bin/env guile
!#

(use-modules (srfi srfi-1))

; compile takes an expression and an environment and returns the BLC
(define (lambda? s)
  (and (symbol? s) (string-prefix? "\\" (symbol->string s))))

(define (lambda->symbol l)
  (string->symbol (substring (symbol->string l) 1)))

(define (display-index i)
  (cond
    ((eq? i 0) (display "0"))
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
  ((lambda? (car e)) (display "00") (compile (cdr e) (acons (lambda->symbol (car e)) (length env) env)))
  ((and (eq? (length e) 1) (symbol? (car e))) (display-index (- (length env) (cdr (assq (car e) env)))))
  ((and (list? e) (eq? (length e) 1)) (compile (car e) env))
  (else              (display "01") (compile (take e (- (length e) 1)) env) (compile (drop e (- (length e) 1)) env))))

(define (read-all)
  (let ((expr (read)))
    (cond
      ((eof-object? expr) '())
      (else (cons expr (read-all))))))

; read and parse stdin, then compile
(compile (read-all) '())
