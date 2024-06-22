#include <array>
#include <boost/make_shared.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <tuple>
#include <vector>

template <size_t SIZE>
struct ListNode
{
	constexpr static size_t BYTES_PER_STEP = 16;

	std::shared_ptr<ListNode> next;

	ListNode() : next(nullptr) {}

    // Yes, this is not strictly correct C++ if SIZE == 0. The compilers
    // accept it though.
	uint8_t data[SIZE * BYTES_PER_STEP];
};

struct Scenario
{
	constexpr static size_t LIST_COUNT = 1000;
	constexpr static size_t AVG_LIST_LENGTH = 1000;
	constexpr static size_t ITERATIONS = LIST_COUNT * 50;
	constexpr static size_t SIZES = 5;

	struct InsertNode
	{
		size_t listNumber;
		size_t sizeClass;
	};

	std::vector<InsertNode> rampUp;

	std::vector<std::vector<InsertNode>> iterations;

	template <size_t SIZE = SIZES - 1>
	void
	showSizes()
	{
		std::cout << "Size " << SIZE << ": " << sizeof(ListNode<SIZE>)
		          << " bytes.\n";
		if constexpr (SIZE > 0) {
			showSizes<SIZE - 1>();
		}
	}

	void
	generate(int seed)
	{
		static std::mt19937 gen(seed);
		static std::uniform_int_distribution<> dis(0, LIST_COUNT - 1);

		// generate ramp-up
		for (size_t s = 0; s < SIZES; ++s) {
			for (size_t i = 0; i < (LIST_COUNT * AVG_LIST_LENGTH); ++i) {
				rampUp.push_back(InsertNode{.listNumber = static_cast<size_t>(dis(gen)),
				                            .sizeClass = s});
			}
		}

		// genarate iterations
		for (size_t i = 0; i < ITERATIONS; ++i) {
			for (size_t s = 0; s < SIZES; ++s) {
				iterations.emplace_back();
				for (size_t j = 0; j < AVG_LIST_LENGTH; ++j) {
					iterations.back().push_back(InsertNode{
					    .listNumber = static_cast<size_t>(dis(gen)), .sizeClass = s});
				}
			}
			std::shuffle(std::begin(iterations[i]), std::end(iterations[i]), gen);
		}
	}
};

template <class NodeBuilder, size_t SIZES>
class Tester {
public:
	Tester(const Scenario & scenarioIn)
	    : heads(), iteration(0), scenario(scenarioIn)
	{
		initLists(Scenario::LIST_COUNT);
	}

	void
	run()
	{
		rampUp();

		auto start = std::chrono::high_resolution_clock::now();
		for (size_t i = 0; i < scenario.iterations.size(); ++i) {
			runIteration(i);
		}
		auto end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> duration = end - start;
		std::cout << "Iterations for " << NodeBuilder::NAME << " took "
		          << duration.count() << " seconds\n";
	}

private:
	template <size_t INNER_SIZES>
	constexpr static auto
	buildTuple()
	{
		if constexpr (INNER_SIZES == 0) {
			return std::tuple<std::vector<std::shared_ptr<ListNode<0>>>>{};
		} else {
			return std::tuple_cat(
			    buildTuple<INNER_SIZES - 1>(),
			    std::tuple<std::vector<std::shared_ptr<ListNode<INNER_SIZES>>>>{});
		}
	}

	template <size_t SIZE = SIZES - 1>
	void
	insert(const Scenario::InsertNode & insertNode)
	{
		if (insertNode.sizeClass == SIZE) {
			auto newNode = NodeBuilder::template build<SIZE>();
			newNode.get()->next = std::get<SIZE>(heads)[insertNode.listNumber];
			std::get<SIZE>(heads)[insertNode.listNumber] = newNode;
		} else {
			if constexpr (SIZE > 0) {
				insert<SIZE - 1>(insertNode);
			}
		}
	}

	template <size_t SIZE = SIZES - 1>
	void
	clearList(size_t listIdx)
	{
		std::get<SIZE>(heads)[listIdx].reset();
		if constexpr (SIZE > 0) {
			clearList<SIZE - 1>(listIdx);
		}
	}

	template <size_t SIZE = SIZES - 1>
	void
	initLists(size_t listCount)
	{
		std::get<SIZE>(heads).resize(listCount, nullptr);
		if constexpr (SIZE > 0) {
			initLists<SIZE - 1>(listCount);
		}
	}

	void
	rampUp()
	{
		for (const Scenario::InsertNode & insertNode : scenario.rampUp) {
			insert(insertNode);
		}
	}

	void
	runIteration(size_t i)
	{
		clearList(i % Scenario::LIST_COUNT);
		for (const Scenario::InsertNode & insertNode : scenario.iterations[i]) {
			insert(insertNode);
		}
	}

	decltype(buildTuple<SIZES - 1>()) heads;
	size_t iteration;

	const Scenario & scenario;
};

struct MakeSharedBuilder
{
	constexpr static const char * NAME = "std::make_shared";

	template <size_t SIZE>
	static std::shared_ptr<ListNode<SIZE>>
	build()
	{
		return std::make_shared<ListNode<SIZE>>();
	}
};

struct NewBuilder
{
	constexpr static const char * NAME = "new";

	template <size_t SIZE>
	static std::shared_ptr<ListNode<SIZE>>
	build()
	{
		return std::shared_ptr<ListNode<SIZE>>(new ListNode<SIZE>());
	}
};

struct FastBoostPoolBuilder
{
	constexpr static const char * NAME = "boost::fast_pool_allocator";

	template <size_t SIZE>
	static std::shared_ptr<ListNode<SIZE>>
	build()
	{
		static boost::fast_pool_allocator<void> alloc;
		return std::allocate_shared<ListNode<SIZE>>(alloc);
	}
};

int
main()
{
	Scenario scenario;
	scenario.showSizes();
	scenario.generate(42);

	// One round to warm up the caches
	{
		Tester<FastBoostPoolBuilder, Scenario::SIZES> fastBoostTester(scenario);
		fastBoostTester.run();
	}

	//
	// Actual tests
	//
	{
		Tester<MakeSharedBuilder, Scenario::SIZES> makeSharedTester(scenario);
		makeSharedTester.run();
	}

	{
		Tester<FastBoostPoolBuilder, Scenario::SIZES> fastBoostTester(scenario);
		fastBoostTester.run();
	}

	{
		Tester<NewBuilder, Scenario::SIZES> newTester(scenario);
		newTester.run();
	}
}
