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
#include <ctime>

template<typename T>
class matrix_t_ {
protected:
    std::size_t height, width;

    matrix_t_(int height, int width) : height(height), width(width) {}

    virtual T &operator()(std::size_t const &, std::size_t const &) = 0;


    virtual T trace() {
        T sum = {};
        for (std::size_t i = 0; i < std::min(height, width); i++) {
            sum += (*this)(i, i);
        }
        return sum;
    }

public:
    auto &operator+(matrix_t_<T> &);

    virtual void print() {
        for (std::size_t i = 0; i < height; i++) {
            for (std::size_t j = 0; j < width; j++)
                std::cout << (*this)(i, j) << "\t";
            std::cout << std::endl;
        }
    }
};


template<typename T>
class matrix_dense : public matrix_t_<T> {
private:
    std::vector<T> data;

public:
    matrix_dense(int height, int width) : matrix_t_<T>(height, width) {
        data = std::vector<T>(height * width);
    }

    T &operator()(std::size_t const &row, std::size_t const &col) override {
        if (row < this->height && col < this->width)
            return this->data[row + (col * this->height)];
        else
            throw std::out_of_range("Out of range.");
    }
};

template<typename T>
class matrix_triangulaire_sup : public matrix_t_<T> {
private:
    std::vector<T> data;
    T valInf;

public:
    matrix_triangulaire_sup(int height, int width, T valInf) : matrix_t_<T>(height, width), valInf(valInf) {
        std::size_t size;
        if (height >= width)
            size = (width * (width + 1) / 2);
        else
            size = (width * (width + 1) / 2) - (std::abs(height - width) * ((std::abs(height - width) + 1)) / 2);
        data = std::vector<T>(size);
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

    T trace() override {
        T sum = {};
        for (std::size_t i = 0, j = 0; i < std::min(this->height, this->width); j += this->width - i, i++) {
            sum += this->data[j];
        }
        return sum;
    }

    void print() override {
        for (std::size_t i = 0; i < this->height; i++) {
            for (std::size_t j = 0; j < this->width; j++) {
                if (i <= j)
                    std::cout << (*this)(i, j) << "\t";
                else
                    std::cout << valInf << "\t";
            }
            std::cout << std::endl;
        }
    }
};

template<typename T>
class matrix_diag : public matrix_t_<T> {
private:
    std::vector<T> data;
    T defaultVal;

public:
    matrix_diag(int height, int width, T defaultVal) : matrix_t_<T>(height, width), defaultVal(defaultVal) {
        data = std::vector<T>(std::min(height, width));
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

    T trace() override {
        T sum = {};
        for (auto const &n : this->data)
            sum += n;
        return sum;
    }


    void print() override {
        for (std::size_t i = 0; i < this->height; i++) {
            for (std::size_t j = 0; j < this->width; j++) {
                if (i == j)
                    std::cout << (*this)(i, j) << "\t";
                else
                    std::cout << defaultVal << "\t";
            }
            std::cout << std::endl;
        }
    }
};

template<typename T>
auto &matrix_t_<T>::operator+(matrix_t_<T> &m1) {
    auto result = new matrix_dense<T>(std::max(m1.height, this->height), std::max(m1.width, this->width));
    for (std::size_t i = 0; i < result->height; i++)
        for (std::size_t j = 0; j < result->width; j++)
            (*result)(i, j) = m1(i, j) + (*this)(i, j);
    return *result;
}


int main() {
    matrix_triangulaire_sup<int> m_triang = {4, 4, 1};

    m_triang(0, 0) = 10;
    m_triang(0, 1) = 11;
    //m_triang(1, 0) = 12;
    m_triang(1, 1) = 13;
    m_triang(2, 2) = 13;

    m_triang.print();

    std::cout << m_triang.trace() << std::endl;

    matrix_diag<int> m_diag = {4, 4, 1};

    m_diag(0, 0) = 10;
    m_diag(1, 1) = 11;
    m_diag(2, 2) = 13;
    m_diag(3, 3) = 13;

    m_diag.print();

    std::cout << m_diag.trace() << std::endl;

    auto m3 = m_triang + m_diag;
    m3.print();

    /*auto begin = std::chrono::high_resolution_clock::now();
    auto trace = m_triang.trace();
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = end - begin;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << ms << std::endl;*/

    return 0;
}
