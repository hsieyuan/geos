#include <geos/geom/util/SineStarFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/profiler.h>
#include <geos/geom/IntersectionMatrix.h>
#include <geos/geom/prep/PreparedGeometryFactory.h>
#include <geos/operation/relateng/RelateNG.h>
#include <geos/operation/relateng/RelatePredicate.h>
#include <geos/io/WKBWriter.h>
#include <BenchmarkUtils.h>

#include <iomanip>

using namespace geos::geom;

std::size_t MAX_ITER = 10;
std::size_t NUM_LINES = 10000;
std::size_t NUM_LINES_PTS = 100;


int testRelateOp(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& lines) {
    int count = 0;
    for (const auto& line : lines) {
        auto im = g.relate(line.get());
        count += im->isIntersects();
    }
    return count;
}

int testGeometryIntersects(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& lines) {
    int count = 0;
    for (const auto& line : lines) {
        count += g.intersects(line.get());
    }
    return count;
}

int testGeometryContains(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& lines) {
    int count = 0;
    for (const auto& line : lines) {
        count += g.contains(line.get());
    }
    return count;
}

int testPrepGeomCached(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& lines) {
    int count = 0;
    auto prep = prep::PreparedGeometryFactory::prepare(&g);
    for (const auto& line : lines) {
        count += prep->intersects(line.get());
    }
    return count;
}

int testRelateNGPreparedCached(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& lines) {
    int count = 0;
    auto prep = geos::operation::relateng::RelateNG::prepare(&g);
    for (const auto& line : lines) {
        count += prep->evaluate(line.get(), *geos::operation::relateng::RelatePredicate::intersects());
    }
    return count;
}

template<typename F>
double test(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms, const std::string& method, F&& fun, double base)
{
    geos::util::Profile sw("PreparedPolygonIntersects");
    sw.start();

    int count = 0;
    for (std::size_t i = 0; i < MAX_ITER; i++) {
        count += fun(g, geoms);
    }

    sw.stop();
    double tot = sw.getTot();
    double timesFaster = base / tot;
    std::cout << std::fixed << std::setprecision(0);
    std::cout << g.getNumPoints() << "," 
        << MAX_ITER * geoms.size() << "," 
        << count << "," << geoms[0]->getGeometryType() << "," 
        << geoms[0]->getNumPoints() << "," 
        << method << "," 
        << tot << ","
        << timesFaster 
        << std::endl;
    return tot;
}

void test (std::size_t npts) {

    auto target = geos::benchmark::createSineStar({0, 0}, 100, npts);
    auto polygons = geos::benchmark::createPolygons(*target->getEnvelopeInternal(), NUM_LINES, 1.0, NUM_LINES_PTS);
    auto lines = geos::benchmark::createLines(*target->getEnvelopeInternal(), NUM_LINES, 1.0, NUM_LINES_PTS);
    auto points = geos::benchmark::createPoints(*target->getEnvelopeInternal(), NUM_LINES);

    double base;
    base = test(*target, polygons, "RelateOp", testRelateOp, 0);
    test(*target, polygons, "Geometry::intersects", testGeometryIntersects, base);
    test(*target, polygons, "PrepGeomCached", testPrepGeomCached, base);
    test(*target, polygons, "RelateNGPreparedCached", testRelateNGPreparedCached, base);

    base = test(*target, lines, "RelateOp", testRelateOp, 0);
    test(*target, lines, "Geometry::intersects", testGeometryIntersects, base);
    test(*target, lines, "PrepGeomCached", testPrepGeomCached, base);
    test(*target, lines, "RelateNGPreparedCached", testRelateNGPreparedCached, base);

    base = test(*target, points, "RelateOp", testRelateOp, 0);
    test(*target, points, "Geometry::intersects", testGeometryIntersects, base);
    test(*target, points, "PrepGeomCached", testPrepGeomCached, base);
    test(*target, points, "RelateNGPreparedCached", testRelateNGPreparedCached, base);
}

int main() {
    std::cout << "target_points,num_tests,num_hits,test_type,pts_in_test,method,time,factor" << std::endl;
    test(5);
    test(10);
    test(500);
    test(1000);
    test(2000);
    test(4000);
    test(8000);
    test(16000);
}
