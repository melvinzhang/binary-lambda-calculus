#!/usr/bin/env guile
!#

(define (lambda->symbol l)
  (string->symbol (substring (symbol->string l) 1)))

(define (decode env)
  (let ((c (read-char)))
    (cond
      ((eof-object? c) #f)
      ((char=? c #\0) (decode-lam-or-app env))
      (else (decode-var 0 env)))))

(define (decode-var d env)
  (let ((c (read-char)))
    (cond
      ((eof-object? c) #f)
      ((char=? c #\0) 
       (if (>= d (length env)) 
         (list d) 
         (list (list-ref env d))))
      (else (decode-var (+ d 1) env)))))

(define (decode-lam-or-app env)
  (let ((c (read-char)))
    (cond
      ((eof-object? c) #f)
      ((char=? c #\0) (decode-lam env))
      (else (list (decode env) (decode env))))))

(define (decode-lam env)
  (let ((x (gensym "\\v")))
    (cons x (decode (cons (lambda->symbol x) env)))))

(define (decode-all)
  (let ((e (decode '())))
    (cond
      (e (display e) (newline) (decode-all))
      (else #f))))

;(display (decode '()))
(decode-all)
