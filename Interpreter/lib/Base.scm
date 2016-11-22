(define (add-list l) (if (null? l) 0 (+ (car l) (add-list (cdr l)))))
(define (- num . args) (+ num (~ (add-list args))))
(define (not expr) (if expr #f #t))
(define (list-and l) (if (null? l) #t (if (car l) (list-and (cdr l)) #f)))
(define (and expr . args) (if expr (list-and args) #f))
(define (list-or l) (if (null? l) #t (if (car l) #t (list-and (cdr l)))))
(define (or expr . args) (if expr #t (list-and args)))
(define (equal x y) (and (not (< x y)) (not (< y x))))
(define (equal-list l) (if (null? (cdr l)) #t (if (equal (car l) (car (cdr l))) (equal-list (cdr l)) #f)))
(define (= expr . args) (if (equal expr (car args)) (equal-list args) #f))
