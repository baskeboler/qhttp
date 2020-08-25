/** @file qhttpheaders.hpp
 *
 * @copyright (C) 2016
 * @date 2016.06.30
 * @author amir zamani <azadkuh@live.com>
 *
 */

#ifndef __QHTTPHEADERS_HPP__
#define __QHTTPHEADERS_HPP__
///////////////////////////////////////////////////////////////////////////////
#include "qhttpfwd.hpp"
#include <QHash>
///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
///////////////////////////////////////////////////////////////////////////////

/** A map of request or response headers. */
class Headers : public QHash<QByteArray, QByteArray> {
public:
  /** checks for a header item, regardless of the case of the characters. */
  bool has(const QByteArray &key) const { return contains(key.toLower()); }

  /** checks if a header has the specified value ignoring the case of the
   * characters. */
  bool keyHasValue(const QByteArray &key, const QByteArray &value) const {
    auto lowerKey = key.toLower();
    if (!contains(lowerKey))
      return false;

    const QByteArray &v = QHash<QByteArray, QByteArray>::value(lowerKey);
    return v.indexOf(value) != -1;
  }

  bool keyHasValueInsensitive(const QByteArray &key,
                              const QByteArray &value) const {
    auto lowerKey = key.toLower();
    if (!contains(lowerKey))
      return false;

    const QByteArray &v = QHash<QByteArray, QByteArray>::value(lowerKey);
    return (v.toLower().indexOf(value.toLower()) != -1);
    // return qstrnicmp(value.constData(), v.constData(), v.size()) == 0;
  }

  template <class Func> void forEach(Func &&f) const {
    qhttp::for_each(constBegin(), constEnd(), f);
  }
  QVariant toVariant() const {
    QVariantHash TempHash;
    for (auto Iter = this->begin(); Iter != this->end(); ++Iter)
      TempHash.insert(Iter.key(), Iter.value());
    return TempHash;
  }

    Headers fromVariant(const QVariant& _value){
      if(_value.canConvert<QVariantHash>()){
        QVariantHash TempHash = _value.value<QVariantHash>();
        this->clear();
        for(auto Iter = TempHash.begin(); Iter != TempHash.end(); ++Iter)
            this->insert(Iter.key().toUtf8(), Iter.value().toByteArray());

        return *this;
      }else
        throw exQHttpBase("specified value can not be converted to QVariantHash");
    }
};

///////////////////////////////////////////////////////////////////////////////
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
#endif // __QHTTPHEADERS_HPP__
