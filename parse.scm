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
  (newline) 
  (display "e  : ")
  (display e) 
  (newline) 
  (display "env: ")
  (display env) 
  (newline) 
  
  (cond
  ((null? e) #f)
  ((lambda? (car e)) (display " 00 ") (compile (cdr e) (acons (lambda->symbol (car e)) (length env) env)))
  ((symbol? (car e)) (display-index (- (length env) (cdr (assq (car e) env)))))
  (else              (display " 01 ") (compile (car e) env) (compile (cdr e) env))))

; apply takes a function and an argument to a value
(define (apply f x)
  (compile (cddr (car f)) (cons (list (cadr (car f)) x) (cdr f))))

; read and parse stdin, then compile
(compile (read) '())
