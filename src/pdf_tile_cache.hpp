#pragma once

#include <list>

#include <QHash>
#include <QImage>
#include <QString>
#include <QtGlobal>

namespace pdf_document_view {

/// Cache key for one device-space tile of a rendered page (see `docs/rendering-threading.md`).
struct TileKey {
    QString docIdentity;
    int pageIndex = 0;
    int zoomMilli = 0;
    int dprMilli = 0;
    int col = 0;
    int row = 0;

    friend bool operator==(const TileKey& a, const TileKey& b) noexcept {
        return a.docIdentity == b.docIdentity && a.pageIndex == b.pageIndex && a.zoomMilli == b.zoomMilli
            && a.dprMilli == b.dprMilli && a.col == b.col && a.row == b.row;
    }
};

inline size_t qHash(const TileKey& k, size_t seed = 0) noexcept {
    seed ^= ::qHash(k.docIdentity) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= ::qHash(k.pageIndex) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= ::qHash(k.zoomMilli) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= ::qHash(k.dprMilli) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= ::qHash(k.col) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= ::qHash(k.row) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

/// LRU of full tile images (device pixels). Evicts oldest insert/access when over capacity.
class TileImageCache {
public:
    explicit TileImageCache(int maxEntries = 64)
        : m_maxEntries(qMax(1, maxEntries)) {}

    void clear() {
        m_lru.clear();
        m_images.clear();
    }

    /// Returns a pointer to the cached image while promoting @p key to most-recently used, or null when missing.
    [[nodiscard]] const QImage* lookup(const TileKey& key) {
        auto it = m_images.find(key);
        if (it == m_images.end()) {
            return nullptr;
        }
        m_lru.splice(m_lru.begin(), m_lru, it.value().lruIt);
        return &it.value().image;
    }

    void insert(const TileKey& key, QImage image) {
        auto it = m_images.find(key);
        if (it != m_images.end()) {
            it.value().image = std::move(image);
            m_lru.splice(m_lru.begin(), m_lru, it.value().lruIt);
            return;
        }

        while (static_cast<int>(m_lru.size()) >= m_maxEntries && !m_lru.empty()) {
            evictOldest();
        }

        m_lru.push_front(key);
        Entry e;
        e.image = std::move(image);
        e.lruIt = m_lru.begin();
        m_images.insert(key, std::move(e));
    }

private:
    struct Entry {
        QImage image;
        std::list<TileKey>::iterator lruIt;
    };

    void evictOldest() {
        if (m_lru.empty()) {
            return;
        }
        const TileKey victim = m_lru.back();
        m_lru.pop_back();
        m_images.remove(victim);
    }

    int m_maxEntries;
    std::list<TileKey> m_lru;
    QHash<TileKey, Entry> m_images;
};

} // namespace pdf_document_view
