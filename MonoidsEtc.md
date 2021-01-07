

https://www.youtube.com/watch?v=giWCdQ7fnQU&t=1515s
https://en.wikibooks.org/wiki/Haskell/Category_theory
https://wiki.haskell.org/Applicative_functor

# Monoid


# Functor

map(func, someFunctor)

template<typename A, typename B>
SomeFunctor<B> map(function<B>(A), SomeFunctor<A>);

SomeFunctor<A> is like a container containing elements a of A.
For example vector<int> is a Functor.

In Haskell:
class Functor (f :: * -> *) where
  fmap :: (a -> b) -> f a -> f b

# Applicative Functor

Is like a functor with two extra operations; pure and apply.

- pure(t) where t is of type T results in a value of type Applicative<T>
- apply(aff, afv) is a legal expression when:
  - aff has type Applicative<F> for some callable F where f(t) is well defined if f is of type F and t is of type T.
  - afv has type Applicative<T> for some type T
  - apply(aff, afv) has type Applicative<U> iff the result type of f(t) is U.

Example optional:
# pure, lifts a value
template <typename T>
optional<T> pure(T t) { return optional<T>(t); }

template <typename T, template U>
optional<U> apply(optional<function<U(T)>> a, optional<T> b) {
	if(a&&b)
		return (*a)(*b);
	else
		return nullopt;
}

http://hackage.haskell.org/package/base-4.14.1.0/docs/src/GHC.Base.html#Applicative


# Monoid

An applicative functor with one extra operation join.

join:  Monad< Monad<T> > -> Monad<T>
