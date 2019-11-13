/*

L'objectif est d'écrire une hiérarchie de classe de matrice: matrice dense, triangulaire sup, diagonale.

1. Écrire une classe matrix_t_ abstraite avec comme interface
	- operator(i, j) virtuelle pure
	- trace() qui renvoit la somme des éléments sur la diagonale
	- print() qui affiche la matrice dans le terminale
	=> trace() et print() sont à implémenter dans matrix_t_

2. Écrire une classe matrix_dense qui hérite de matrice_t_ et implémente operator(i,j)

3. Écrire une classe matrix_triangulaire_sup qui hérite de matrice_t_ et implémente operator(i,j)

4. Écrire une classe matrix_diagonal qui hérite de matrice_t_ et implémente operator(i,j)

5. En utilisant std::chrono, effetuer des mesures de performances de l'implémentation de trace() et print() 
dans le cas des matrices diagonales et triangulaires pour des tailles de matrices croissantes. Commenter les résultats
obtenus.
 Lorsque les matrices deviennent plus grande, on obtient naturellement un temps de calcul plus long pour la trace.
 Cependant, on remarque le calcul de la trace de la matrice triangulaire superieur devient vite beaucoup plus long
 que celui de la matrice diagonale. En effet, Nous faisons beaucoup plus d'appels aux valeurs contenues dans la matrice
 diagonale car la partie haute de celle-ci contient des valeurs inconnues contrairement a la matrice triangulaire.

6. Rendre trace() et print() virtuelle dans matrix_t_ et les ré-implémenter pour les classes filles

7. Discuter si operator()(i,j) a du sens pour "écrire" dans une matrice non dense ?
 operator()(i,j) a du sens pour une matrice dense car on veut pouvoir acceder a chacune des donnes du tableau. En
 revanche, il n'est pas tres coherent pour une matrice diagonale. En effet, seul les elements de la diagonale sont
 interessants et nous pouvons les recuperer facilement avec seulement l'index dans la diagonale. Cet operator est
 cependant plus interessant pour une matrice triangulaire car on doit pouvoir acceder a la moitie des elements de la
 matrice (ici la moitie haute).

8. Implémenter une fonction virtuelle add qui prend deux matrices de types quelconques et qui renvoit une matrice
du bon type contenant la somme des éléments des deux matrices. Comment résoudre la problème de connaissance des types 
des deux matrices d'entrées pour en déduire le type de sortie ? Quel outil du standard pouvez vous utiliser pour renvoyer
un objet polymorphe depuis cette fonction membre ?

*/
#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <numeric>
#include <complex>

template<typename T>
class matrix_dense;

template<typename T>
class matrix_triangulaire_sup;

template<typename T>
class matrix_diag;

template<typename T>
class matrix_t_ {
protected:
    std::size_t height;
    std::size_t width;
    std::vector<T> data;

public:
    matrix_t_(int height, int width) : height(height), width(width) {}

    virtual T &operator()(std::size_t const &, std::size_t const &) = 0;

    virtual const T &operator()(std::size_t const &, std::size_t const &) const = 0;

    virtual void print() {
        for (std::size_t i = 0; i < height; i++) {
            for (std::size_t j = 0; j < width; j++)
                std::cout << (*this)(i, j) << "\t";
            std::cout << std::endl;
        }
    }

    virtual T trace() {
        T sum = {};
        for (std::size_t i = 0; i < std::min(height, width); i++) {
            sum += (*this)(i, i);
        }
        return sum;
    }
};


template<typename T>
class matrix_dense : public matrix_t_<T> {

public:
    matrix_dense(int height, int width) : matrix_t_<T>(height, width) {
        this->data = std::vector<T>(height * width);
    }

    T &operator()(std::size_t const &row, std::size_t const &col) override {
        if (row < this->height && col < this->width)
            return this->data[row + (col * this->height)];
        else
            throw std::out_of_range("Out of range.");
    }

    const T &operator()(std::size_t const &row, std::size_t const &col) const override {
        if (row < this->height && col < this->width)
            return this->data[row + (col * this->height)];
        else
            throw std::out_of_range("Out of range.");
    }
};

template<typename T>
class matrix_triangulaire_sup : public matrix_t_<T> {
private:
    T valInf;

public:
    matrix_triangulaire_sup(int height, int width, T valInf) : matrix_t_<T>(height, width), valInf(valInf) {
        std::size_t size;
        if (height >= width)
            size = (width * (width + 1) / 2);
        else
            size = (width * (width + 1) / 2) - (std::abs(height - width) * ((std::abs(height - width) + 1)) / 2);
        this->data = std::vector<T>(size);
    }

    T &operator()(std::size_t const &row, std::size_t const &col) override {
        if (row < this->height && col < this->width) {
            if (row > col)
                return valInf;
            else {
                return this->data[col + row * this->width - (row * (row + 1)) / 2];
            }
        } else
            throw std::out_of_range("Out of range.");
    }

    const T &operator()(std::size_t const &row, std::size_t const &col) const override {
        if (row < this->height && col < this->width) {
            if (row > col)
                return valInf;
            else {
                return this->data[col + row * this->width - (row * (row + 1)) / 2];
            }
        } else
            throw std::out_of_range("Out of range.");
    }
};

template<typename T>
class matrix_diag : public matrix_t_<T> {
private:
    T defaultVal;

public:
    matrix_diag(int height, int width, T defaultVal) : matrix_t_<T>(height, width), defaultVal(defaultVal) {
        this->data = std::vector<T>(std::min(height, width));
    }

    T &operator()(std::size_t const &row, std::size_t const &col) override {
        if (row < this->height && col < this->width) {
            if (row != col)
                return defaultVal;
            else {
                return this->data[row];
            }
        } else
            throw std::out_of_range("Out of range.");
    }

    const T &operator()(std::size_t const &row, std::size_t const &col) const override {
        if (row < this->height && col < this->width) {
            if (row != col)
                return defaultVal;
            else {
                return this->data[row];
            }
        } else
            throw std::out_of_range("Out of range.");
    }

    T getDefaultVal() const {
        return defaultVal;
    }
};

void testPerformance() {

    for (unsigned int i = 10; i <= 14; i++) {
        int size = std::pow(2, i);
        matrix_dense<int> m_dense = {size, size};
        matrix_triangulaire_sup<int> m_triang = {size, size, 0};
        matrix_diag<int> m_diag = {size, size, 0};

        std::cout << "======SIZE " << size << 'x' << size << "========" << std::endl;

        auto start = std::chrono::steady_clock::now();
        auto trace = m_dense.trace();
        auto end = std::chrono::steady_clock::now();

        std::cout << "Trace matrix dense of : " << trace << ", calculated in "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
                  << " µs" << std::endl;

        start = std::chrono::steady_clock::now();
        trace = m_triang.trace();
        end = std::chrono::steady_clock::now();

        std::cout << "Trace matrix triang sup : " << trace << ", calculated in "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
                  << " µs" << std::endl;
        start = std::chrono::steady_clock::now();
        trace = m_diag.trace();
        end = std::chrono::steady_clock::now();

        std::cout << "Trace matrix diagonal : " << trace << ", calculated in "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
                  << " µs" << std::endl;

        std::cout << std::endl;
    }
}


int main() {

    testPerformance();

    /*
        ======SIZE 1024x1024========
        Trace matrix dense of : 0, calculated in 52 µs
        Trace matrix triang sup : 0, calculated in 29 µs
        Trace matrix diagonal : 0, calculated in 11 µs

        ======SIZE 2048x2048========
        Trace matrix dense of : 0, calculated in 116 µs
        Trace matrix triang sup : 0, calculated in 106 µs
        Trace matrix diagonal : 0, calculated in 23 µs

        ======SIZE 4096x4096========
        Trace matrix dense of : 0, calculated in 214 µs
        Trace matrix triang sup : 0, calculated in 225 µs
        Trace matrix diagonal : 0, calculated in 45 µs

        ======SIZE 8192x8192========
        Trace matrix dense of : 0, calculated in 440 µs
        Trace matrix triang sup : 0, calculated in 434 µs
        Trace matrix diagonal : 0, calculated in 89 µs

        ======SIZE 16384x16384========
        Trace matrix dense of : 0, calculated in 1169 µs
        Trace matrix triang sup : 0, calculated in 897 µs
        Trace matrix diagonal : 0, calculated in 222 µs
    */

    return 0;
}
