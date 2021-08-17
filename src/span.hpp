
namespace sudoku {

/** Container for an iterator range */
template<typename T>
class span
{
public:
    span() {
    }

    span(T && begin, T && end)
        : begin_{std::move(begin)},
          end_{std::move(end)} {
    }

    T & begin() {
        return begin_;
    }

    T & end() {
        return end_;
    }

private:
    T begin_;
    T end_;
};

}