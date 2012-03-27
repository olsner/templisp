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

(define (read) '(cons 1 2))
(define (read2)
  (let ((c (whitespace-getc)))
    (cond
      ((= c 40) (read-list)) ; '('
      ((symbol-char1 c) (let ((sym (cons c (read-symbol)))) (display sym) sym))
      ((elem c digits) (read-number c))
      ((null? c) (error "eof" c))
      (else (progn (display (cons "read fallback" c)) c)))))
(define (read-list)
  (display "read-list")
  (let ((res (cons nil nil)))
    (read-rest res)
    (display (cdr res))
    (cdr res)))
(define (read-end-paren)
  (display "read-end-paren")
  (whitespace-getc))
(define (read-rest target)
  (display "read-rest")
  (let ((c (whitespace-getc)))
    (cond
      ((= c 46) (progn (set-cdr! target (read)) (read-end-paren)))
      ((= c 41) (display "read-rest found ')'"))
      (else
        (progn
          (ungetc c)
          (let ((res (cons (read) nil)))
              (display (cons "read-rest has read a car" (car res)))
              (set-cdr! target res)
              (read-rest res)))))))
(define (read-symbol)
  (display "read-symbol")
  (let ((c (getc)))
    (cond
      ((symbol-charn c) (cons c (read-symbol)))
      (else (progn (ungetc c) nil)))))
(define (read-number c)
  (let ((n (lookc)))
    (if (elem n digits) (cons c (read-number n)) (list c))))

(define (whitespace-getc)
  (let ((c (getc)))
    (cond
      ((or (= c 10) (= c 32)) (whitespace-getc))
      ((= c 59) (eat-comment-getc))
      (else c))))
(define (eat-comment-getc)
  (display "eat-comment-getc")
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

(define (map fun list)
  (if (null? list) nil (cons (fun (car list)) (map fun (cdr list)))))
(define (cadr x) (car (cdr x)))
(define (length list)
  (if (null? list) 0 (+ 1 (length (cdr list)))))
(define = eq?)
(define true 't)
(define false nil)
(define else 't)
(define (or x y) (if x 't y))
(define (set-car! x y) (set! (car x) y))
(define (set-cdr! x y) (set! (cdr x) y))
(define (newline) (putc 10))
(define (elem x list)
  (cond
    ((null? list) nil)
    ((eq? (car list) x) 't)
    (else (elem x (cdr list)))))

(define (error msg exp) (display msg) (display exp) (abort))

; from ch4-mceval.scm
(define the-global-environment (setup-environment))
(driver-loop)
