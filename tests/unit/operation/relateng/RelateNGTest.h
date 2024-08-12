
// geos
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/geom/Geometry.h>
#include <geos/operation/relateng/RelateNG.h>
#include <geos/operation/relateng/RelatePredicate.h>
#include <geos/operation/relateng/RelateMatrixPredicate.h>
#include <geos/operation/relateng/IntersectionMatrixPattern.h>

// std
#include <memory>

using namespace geos::geom;
using namespace geos::operation::relateng;
using geos::io::WKTReader;
using geos::io::WKTWriter;

namespace tut {
//
// Test Group
//

// Common data used by all tests
struct test_relateng_support {

    WKTReader r;
    WKTWriter w;

    void checkPrepared(const std::string& wkta, const std::string& wktb)
    {
        std::unique_ptr<Geometry> a = r.read(wkta);
        std::unique_ptr<Geometry> b = r.read(wktb);
        auto prep_a = RelateNG::prepare(a.get());

        ensure_equals("equalsTopo", prep_a->equalsTopo(b.get()), a->equals(b.get()));
        ensure_equals("intersects", prep_a->intersects(b.get()), a->intersects(b.get()));
        ensure_equals("disjoint",   prep_a->disjoint(b.get()),   a->disjoint(b.get()));
        ensure_equals("covers",     prep_a->covers(b.get()),     a->covers(b.get()));
        ensure_equals("coveredby",  prep_a->coveredBy(b.get()),  a->coveredBy(b.get()));
        ensure_equals("within",     prep_a->within(b.get()),     a->within(b.get()));
        ensure_equals("contains",   prep_a->contains(b.get()),   a->contains(b.get()));
        ensure_equals("crosses",    prep_a->crosses(b.get()),    a->crosses(b.get()));
        ensure_equals("touches",    prep_a->touches(b.get()),    a->touches(b.get()));
        ensure_equals("relate",     prep_a->relate(b.get()),   a->relate(b.get())->toString());
    }

    void checkIntersectsDisjoint(const std::string& wkta, const std::string& wktb, bool expectedValue)
    {
        checkPredicate(*RelatePredicate::intersects(), wkta, wktb, expectedValue);
        checkPredicate(*RelatePredicate::intersects(), wktb, wkta, expectedValue);
        checkPredicate(*RelatePredicate::disjoint(), wkta, wktb, ! expectedValue);
        checkPredicate(*RelatePredicate::disjoint(), wktb, wkta, ! expectedValue);
    }

    void checkContainsWithin(const std::string& wkta, const std::string& wktb, bool expectedValue)
    {
        checkPredicate(*RelatePredicate::contains(), wkta, wktb, expectedValue);
        checkPredicate(*RelatePredicate::within(),   wktb, wkta, expectedValue);
    }

    void checkCoversCoveredBy(const std::string& wkta, const std::string& wktb, bool expectedValue)
    {
        checkPredicate(*RelatePredicate::covers(),    wkta, wktb, expectedValue);
        checkPredicate(*RelatePredicate::coveredBy(), wktb, wkta, expectedValue);
    }

    void checkCrosses(const std::string& wkta, const std::string& wktb, bool expectedValue)
    {
        checkPredicate(*RelatePredicate::crosses(), wkta, wktb, expectedValue);
        checkPredicate(*RelatePredicate::crosses(), wktb, wkta, expectedValue);
    }

    void checkOverlaps(const std::string& wkta, const std::string& wktb, bool expectedValue)
    {
        checkPredicate(*RelatePredicate::overlaps(), wkta, wktb, expectedValue);
        checkPredicate(*RelatePredicate::overlaps(), wktb, wkta, expectedValue);
    }

    void checkTouches(const std::string& wkta, const std::string& wktb, bool expectedValue)
    {
        checkPredicate(*RelatePredicate::touches(), wkta, wktb, expectedValue);
        checkPredicate(*RelatePredicate::touches(), wktb, wkta, expectedValue);
    }

    void checkEquals(const std::string& wkta, const std::string& wktb, bool expectedValue)
    {
        checkPredicate(*RelatePredicate::equalsTopo(), wkta, wktb, expectedValue);
        checkPredicate(*RelatePredicate::equalsTopo(), wktb, wkta, expectedValue);
    }

    void checkRelate(const std::string& wkta, const std::string& wktb, const std::string expectedValue)
    {
        std::unique_ptr<Geometry> a = r.read(wkta);
        std::unique_ptr<Geometry> b = r.read(wktb);
        RelateMatrixPredicate pred;
        // TopologyPredicate predTrace = trace(pred);
        RelateNG::relate(a.get(), b.get(), pred);
        std::string actualVal = pred.getIM()->toString();
        if (actualVal != expectedValue) {
            std::cerr << std::endl << w.write(*a) << " relate " << w.write(*b) << " = " << actualVal << std::endl;
        }
        ensure_equals("checkRelate", actualVal, expectedValue);
    }

    void checkRelateMatches(const std::string& wkta, const std::string& wktb, const std::string pattern, bool expectedValue)
    {
        auto pred = RelatePredicate::matches(pattern);
        checkPredicate(*pred, wkta, wktb, expectedValue);
    }

    void checkPredicate(TopologyPredicate& pred, const std::string& wkta, const std::string& wktb, bool expectedValue)
    {
        std::unique_ptr<Geometry> a = r.read(wkta);
        std::unique_ptr<Geometry> b = r.read(wktb);
        // TopologyPredicate predTrace = trace(pred);
        bool actualVal = RelateNG::relate(a.get(), b.get(), pred);
        if (actualVal != expectedValue) {
            std::cerr << std::endl << w.write(*a) << " " << pred << " " << w.write(*b) << " = " << actualVal << std::endl;
        }
        ensure_equals("checkPredicate", actualVal, expectedValue);
    }

};


} // namespace tut
