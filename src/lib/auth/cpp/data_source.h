// Copyright (C) 2009  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

// $Id$

#ifndef __DATA_SOURCE_H
#define __DATA_SOURCE_H

#include <vector>

#include <dns/name.h>
#include <dns/rrclass.h>

namespace isc {

namespace dns {
class Name;
class RRType;
class RRset;
class RRsetList;
}

namespace auth {

class NameMatch;
class Query;

class AbstractDataSrc {
    ///
    /// \name Constructors, Assignment Operator and Destructor.
    ///
    /// Note: The copy constructor and the assignment operator are intentionally
    /// defined as private to make it explicit that this is a pure base class.
private:
    AbstractDataSrc(const AbstractDataSrc& source);
    AbstractDataSrc& operator=(const AbstractDataSrc& source);
protected:
    /// \brief The default constructor.
    ///
    /// This is intentionally defined as \c protected as this base class should
    /// never be instantiated (except as part of a derived class).
    AbstractDataSrc() {}
public:
    /// \brief The destructor.
    virtual ~AbstractDataSrc() {};
    //@}

    enum Result {
        SUCCESS,
        ERROR,
        NOT_IMPLEMENTED
    };

    // These flags indicate conditions encountered while processing a query.
    //
    // REFERRAL:       The node contains an NS record
    // CNAME_FOUND:    The node contains a CNAME record
    // NAME_NOT_FOUND: The node does not exist in the data source.
    // TYPE_NOT_FOUND: The node does not contain the requested RRType
    // NO_SUCH_ZONE:   The zone does not exist in this data source.
    enum QueryResponseFlags {
        REFERRAL = 0x01,
        CNAME_FOUND = 0x02,
        NAME_NOT_FOUND = 0x04,
        TYPE_NOT_FOUND = 0x08,
        NO_SUCH_ZONE = 0x10
    };

    // 'High-level' methods.  These will be implemented by the
    // general DataSrc class, and SHOULD NOT be overwritten by subclasses.
    virtual void doQuery(Query& query) = 0;

    // XXX: High-level methods to be implemented later:
    // virtual void doUpdate(Update update) = 0;
    // virtual void doXfr(Query query) = 0;

    // 'Medium-level' methods.  This will be implemented by the general
    // DataSrc class but MAY be overwritten by subclasses.
    virtual void findClosestEnclosure(NameMatch& match) const = 0;

    // Optional 'low-level' methods.  These will have stub implementations
    // in the general DataSrc class but MAY be overwritten by subclasses
    virtual Result init() = 0;
    virtual Result close() = 0;

    // Mandatory 'low-level' methods: These will NOT be implemented by
    // the general DataSrc class; subclasses MUST implement them.
    virtual Result findRRset(const Query& q,
                             const isc::dns::Name& qname,
                             const isc::dns::RRClass& qclass,
                             const isc::dns::RRType& qtype,
                             isc::dns::RRsetList& target,
                             uint32_t& flags,
                             isc::dns::Name* zone = NULL) const = 0;

    virtual Result findExactRRset(const Query& q,
                                  const isc::dns::Name& qname,
                                  const isc::dns::RRClass& qclass,
                                  const isc::dns::RRType& qtype,
                                  isc::dns::RRsetList& target,
                                  uint32_t& flags,
                                  isc::dns::Name* zone = NULL) const = 0;

    // These will have dumb implementations in the general DataSrc
    // class, and SHOULD be overwritten by subclasses.
    virtual Result findAddrs(const Query& q,
                             const isc::dns::Name& qname,
                             const isc::dns::RRClass& qclass,
                             isc::dns::RRsetList& target,
                             uint32_t& flags,
                             isc::dns::Name* zone = NULL) const = 0;

     virtual Result findReferral(const Query& q,
                                const isc::dns::Name& qname,
                                const isc::dns::RRClass& qclass,
                                isc::dns::RRsetList& target,
                                uint32_t& flags,
                                isc::dns::Name* zone = NULL) const = 0;

    // This MUST be implemented by concrete data sources which support
    // DNSSEC, but is optional for others (e.g., the static data source).
    virtual Result findPreviousName(const Query& q,
                                    const isc::dns::Name& qname,
                                    isc::dns::Name& target,
                                    isc::dns::Name* zone) const = 0;

};

// Base class for a DNS Data Source
class DataSrc : public AbstractDataSrc {
    ///
    /// \name Constructors, Assignment Operator and Destructor.
    ///
    /// Note: The copy constructor and the assignment operator are intentionally
    /// defined as private.
private:
    DataSrc(const DataSrc& source);
    DataSrc& operator=(const DataSrc& source);
public:
    DataSrc() : rrclass(isc::dns::RRClass::IN()) {}
    DataSrc(const isc::dns::RRClass& c) : rrclass(c) {}
    /// \brief The destructor.
    virtual ~DataSrc() {};
    //@}

    virtual void doQuery(Query& q);

    virtual void findClosestEnclosure(NameMatch& match) const = 0;

    const isc::dns::RRClass& getClass() const { return rrclass; }
    void setClass(isc::dns::RRClass& c) { rrclass = c; }
    void setClass(const isc::dns::RRClass& c) { rrclass = c; }

    Result init() { return NOT_IMPLEMENTED; }
    Result close() { return NOT_IMPLEMENTED; }

    virtual Result findRRset(const Query& q,
                             const isc::dns::Name& qname,
                             const isc::dns::RRClass& qclass,
                             const isc::dns::RRType& qtype,
                             isc::dns::RRsetList& target,
                             uint32_t& flags,
                             isc::dns::Name* zone = NULL) const = 0;

    virtual Result findExactRRset(const Query& q,
                                  const isc::dns::Name& qname,
                                  const isc::dns::RRClass& qclass,
                                  const isc::dns::RRType& qtype,
                                  isc::dns::RRsetList& target,
                                  uint32_t& flags,
                                  isc::dns::Name* zone = NULL) const = 0;

    virtual Result findAddrs(const Query& q,
                               const isc::dns::Name& qname,
                               const isc::dns::RRClass& qclass,
                               isc::dns::RRsetList& target,
                               uint32_t& flags,
                             isc::dns::Name* zone = NULL) const;

    virtual Result findReferral(const Query& q,
                                const isc::dns::Name& qname,
                                const isc::dns::RRClass& qclass,
                                isc::dns::RRsetList& target,
                                uint32_t& flags,
                                isc::dns::Name* zone = NULL) const;

    virtual Result findPreviousName(const Query& q,
                                    const isc::dns::Name& qname,
                                    isc::dns::Name& target,
                                    isc::dns::Name* zone) const = 0;
private:
    isc::dns::RRClass rrclass;
};

class MetaDataSrc : public DataSrc {
    ///
    /// \name Constructors, Assignment Operator and Destructor.
    ///
    /// Note: The copy constructor and the assignment operator are intentionally
    /// defined as private.
    //@{
private:
    MetaDataSrc(const MetaDataSrc& source);
    MetaDataSrc& operator=(const MetaDataSrc& source);
public:
    MetaDataSrc() : DataSrc(isc::dns::RRClass::ANY()) {}
    MetaDataSrc(const isc::dns::RRClass& c) : DataSrc(c) {}
    /// \brief The destructor.
    virtual ~MetaDataSrc() {}
    //@}

    void addDataSrc(DataSrc* ds);
    void findClosestEnclosure(NameMatch& match) const;

    // Actual queries for data should not be sent to a MetaDataSrc object,
    // so we return NOT_IMPLEMENTED if we receive any.
    //
    // The proper way to use the MetaDataSrc is to run findClosestEnclosure()
    // to get a pointer to the best concrete data source for the specified
    // zone, then send all queries directly to that data source.

    Result findRRset(const Query& q, const isc::dns::Name& qname,
                     const isc::dns::RRClass& qclass,
                     const isc::dns::RRType& qtype,
                     isc::dns::RRsetList& target, uint32_t& flags,
                     isc::dns::Name* zone = NULL) const
    {
        return (NOT_IMPLEMENTED);
    }

    Result findExactRRset(const Query& q, const isc::dns::Name& qname,
                          const isc::dns::RRClass& qclass,
                          const isc::dns::RRType& qtype,
                          isc::dns::RRsetList& target, uint32_t& flags,
                          isc::dns::Name* zone = NULL) const
    {
        return (NOT_IMPLEMENTED);
    }

    Result findAddrs(const Query& q, const isc::dns::Name& qname,
                     const isc::dns::RRClass& qclass,
                     isc::dns::RRsetList& target, uint32_t& flags,
                     isc::dns::Name* zone = NULL) const
    {
        return (NOT_IMPLEMENTED);
    }

    Result findReferral(const Query& q, const isc::dns::Name& qname,
                        const isc::dns::RRClass& qclass,
                        isc::dns::RRsetList& target, uint32_t& flags,
                        isc::dns::Name* zone = NULL) const
    {
        return (NOT_IMPLEMENTED);
    }

    virtual Result findPreviousName(const Query& q,
                                    const isc::dns::Name& qname,
                                    isc::dns::Name& target,
                                    isc::dns::Name* zone) const
    {
        return (NOT_IMPLEMENTED);
    }

private:
    std::vector<DataSrc*> data_sources;
};

class NameMatch {
public:
    NameMatch(const isc::dns::Name& qname) :
        closest_name_(NULL), best_source_(NULL), qname_(qname) {}
    ~NameMatch();

    void update(const DataSrc& new_source, const isc::dns::Name& container);

    const isc::dns::Name& qname() { return (qname_); }
    const isc::dns::Name* closestName() { return (closest_name_); }
    const DataSrc* bestDataSrc() { return (best_source_); }

private:
    const isc::dns::Name* closest_name_;
    const DataSrc* best_source_;
    const isc::dns::Name qname_;
};

}
}

#endif

// Local Variables: 
// mode: c++
// End: 
