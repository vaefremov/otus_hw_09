#pragma once

#include <list>
#include <memory>
#include <string>
#include <boost/filesystem.hpp>

namespace OTUS
{

enum class EventKind
{
    END_STREAM, REGULAR_FILE
};

struct Event
{
    EventKind kind;
    size_t filesize;
    boost::filesystem::path path;
};

constexpr char const* event_type_name(EventKind t)
{
    switch (t)
    {
    case EventKind::END_STREAM:
        return "End of stream";
    case EventKind::REGULAR_FILE :
        return "Regular file";
    default:
        return "Unknown event type!";
    }
}


class IObserver 
{
    public:
    virtual ~IObserver() = default;
    virtual void update(Event const&) = 0;

};

class IObservable 
{
    public:
    virtual ~IObservable() = default;
    virtual void subscribe(std::shared_ptr<IObserver> const&) = 0;
};


class BaseObservable : public IObservable {
public:
    void subscribe(const std::shared_ptr<IObserver>& obs) override {
        m_subscribers.emplace_back(obs);
    }

    void notify(Event const& ev) {
        auto iter = std::begin(m_subscribers);
        while(iter != std::end(m_subscribers)) {
            auto ptr = iter->lock();
            if (ptr) {
                ptr->update(ev);
                ++iter;
            } else {
                // The current implementation explicitly uses the std::list behaviour regarding 
                // the influence of the erase method on iterators (see e.g. https://en.cppreference.com/w/cpp/container/list/erase)
                iter = m_subscribers.erase(iter);
            }
        }
    }
private:
    std::list<std::weak_ptr<IObserver>> m_subscribers;
};

} // namespace OTUS