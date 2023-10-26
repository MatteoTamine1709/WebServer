#ifndef RESULT_H_
#define RESULT_H_

#include <string>

template <typename T>
struct Ok {
    T value;
};

template <typename T>
struct Err {
    T error;
};

template <typename T, typename V = std::string>
struct Result {
   private:
    union {
        Ok<T> m_ok;
        Err<V> m_err;
    };

    bool m_isOk;

   public:
    Result(Ok<T> ok) : m_isOk(true), m_ok(ok) {}
    Result(Err<V> err) : m_isOk(false), m_err(err) {}
    ~Result() {
        if (m_isOk) {
            m_ok.~Ok();
        } else {
            m_err.~Err();
        }
    }

    T unwrap() const {
        if (m_isOk) {
            return std::move(m_ok.value);
        } else {
            throw m_err.error;
        }
    }

    V unwrapErr() const {
        if (!m_isOk) {
            return std::move(m_err.error);
        } else {
            throw m_ok.value;
        }
    }

    bool isErr() const { return !m_isOk; }
    bool isOk() const { return m_isOk; }

    T ok() { return m_ok.value; }
    V err() { return m_err.error; }

    const T& ok() const { return m_ok.value; }
    const V& err() const { return m_err.error; }

    static Result<T, V> ok(T value) { return Result<T, V>(Ok<T>{value}); }
    static Result<T, V> err(V error) { return Result<T, V>(Err<V>{error}); }

    // cast to bool
    operator bool() { return isOk; }
};

#endif