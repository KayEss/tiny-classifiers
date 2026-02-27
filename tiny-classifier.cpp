/**
{{links categories
    Categories:/AI and LLMs
    Categories:/C++
    Categories:/Exposition
    Categories:/LLMs}}
{{field created 2025-08-18T07:20:42.993Z}}
{{field lastUpdate 2026-02-27T06:12:35.710Z}}
{{field displayName Tiny Classifiers/tiny-classifier.cpp — Our First Tiny
Classifier}}
*/
/**
 *
 * My first ever paid programming work, way back in 1985 {{include
 * Acronym:/CE.md}}, was in neural networks. Since then I have not kept up with
 * it. It's more than overdue that I rectify that, and the good news is that
 * [Welch Labs](https://www.youtube.com/@WelchLabsVideo) have put out some
 * Youtube videos, going through the basics slowly enough that even I could
 * follow them.
 *
 * <aside class="sidebar">
 *
 * The videos are:
 *
 * 1. [The Misconception that Almost Stopped
 * AI \[How Models Learn Part 1\]](https://www.youtube.com/watch?v=NrO20Jb-hy0)
 * 2. [The F=ma of Artificial Intelligence
 * \[Backpropagation\]](https://www.youtube.com/watch?v=VkHfRKewkWw){{footnote}}
 * 3. [Why Deep Learning Works Unreasonably
 * Well](https://www.youtube.com/watch?v=qx7hirqgfuU)
 *
 * The code in this page attempts to replicate the results in the second video.
 *
 * </aside>
 *
 * {{footnote
 * Welch Labs have now kindly linked this page from their video as well.
 * }}
 *
 * The second video in particular, looked interesting as it contains, what I
 * think is, more than enough detail to write some code to replicate what is
 * showcased in the video.
 *
 * In this file{{footnote}} I've tried to write a replication of what is shown
 * in that second video. I've done it in C++ with some explanations of what's
 * happening, but not as much or as thoroughly as in the video. _You should
 * certainly watch the Welch Labs videos first_.
 *
 * {{footnote
 * Not all of the required code is in this file. There's also a few
 * extra headers with some utilities in them. I feel including these here would
 * detract from the main narrative I want to tell, so I've pushed them into the
 * other headers. They will be mentioned as we use introduce that
 * functionality.
 *
 * In any case [the source code for everything needed to build and run this code
 * is available on
 * Github](https://github.com/KayEss/tiny-classifiers/blob/main/tiny-classifier.cpp).
 * }}
 *
 * As usual, mistakes are all my own. I think this code is an accurate
 * implementation of what is shown in the videos, but _caveat lector_.
 *
 *
 * ## The models
 *
 * As in Welch Lab's video, we're only going to consider two examples. In the
 * first case we'll look only at the longitude for three cities, and in the
 * second we'll look at both longitude and latitude for four cities.
 *
 */
#include <cstddef>

template<std::size_t Inputs, std::size_t Cities>
struct parameters;

using one_d = parameters<1, 3>;
using two_d = parameters<2, 4>;
/**
 *
 * For the one dimensional case we'll only consider three of the cities.
 * Including the fourth city requires us to use a second dimension in the
 * network and both coordinates of the city locations.
 *
 * The code will handle both cases without modification{{footnote}} so we won't
 * need to re-write anything to run both models.
 *
 * {{footnote
 * There is only one place where we need different code, and we can get the
 * compiler to choose the correct code for us.
 * }}
 *
 * ## Training data
 *
 * In many ways it's nice to use a struct for our geolocation, but it's likely
 * more flexible in terms of solving other problems to consider the input as an
 * array of input values. This gives us the following definition for `geopos`
 * and the four example cities.
 *
 */
#include <array>
#include <tuple>

using geopos = std::array<float, 2>;

auto constexpr cities = std::array{
        std::tuple{
                "Madrid",
                std::array{
                        geopos{40.4167f, -3.7033f}, geopos{40.4153f, -3.6835f},
                        geopos{40.4180f, -3.7143f}, geopos{40.4138f, -3.6921f},
                        geopos{40.4169f, -3.7033f}}},
        std::tuple{
                "Paris",
                std::array{
                        geopos{48.8575f, 2.3514f}, geopos{48.8584f, 2.2945f},
                        geopos{48.8530f, 2.3499f}, geopos{48.8606f, 2.3376f},
                        geopos{48.8606f, 2.3522f}}},
        std::tuple{
                "Berlin",
                std::array{
                        geopos{52.5200f, 13.4050f}, geopos{52.5163f, 13.3777f},
                        geopos{52.5169f, 13.4019f}, geopos{52.5074f, 13.3904f},
                        geopos{52.5251f, 13.3694f}}},
        /**
         * The video doesn't show us any data for Barcelona, so these are some
         * locations grabbed from a map.
         */
        std::tuple{
                "Barcelona",
                std::array{
                        geopos{41.3666f, 2.1599f}, geopos{41.4056f, 2.1746f},
                        geopos{41.4148f, 2.1307f}, geopos{41.4125f, 2.2210f},
                        geopos{41.3835f, 2.1226f}}}};
/**
 *
 * ### Choosing a training example
 *
 * In order to train the neural network, we need to pick random locations in our
 * training data, run them through the network, and see what city it thinks the
 * location belongs to. We can then use that result to adjust the model in order
 * to make it more accurate next time. With enough repetitions, the model will
 * keep giving correct answers.
 *
 * {{footnote
 * The file [_random.hpp_](./random.hpp) contains `pick_index` that allows us to
 * pick a  random index into an array. It also contains random_weight`, which
 * we'll use later on.
 * }}
 *
 * We're going to need to choose random `geopos`es from our `cities` structure
 * above. `pick_random_geopos` will return a random `geopos` for a given city
 * index. The definition for `pick_index` isn't particularly interesting, nor is
 * setting up the random number generator{{footnote}}.
 */
#include "random.hpp"

auto pick_random_geopos(std::size_t const city_index) {
    auto const &positions = std::get<1>(cities[city_index]);
    return positions[pick_index(positions.size())];
}
/**
 *
 * A type that records our training example will help us keep the code later on
 * clear. This `example` type will hold the city that we've picked together with
 * the `geopos` values that we're using.
 *
 */
template<std::size_t Inputs, std::size_t Cities>
struct example {
    /**
     * We will always pick a random city using one of our random number
     * generator functions.
     */
    std::size_t city_index{pick_index(Cities)};
    /**
     * For the case where `Inputs` is one we pick the second number from the
     * location array (which corresponds to the longitude), and for the second
     * case we can just copy the array value. The `if constexpr` allows us to
     * write some logic that runs at compile time to handle this difference. C++
     * generally limits us to a single return type, but because this code runs
     * at compile time we only compile the branch we actually need. As the other
     * branch is ignored it's return type doesn't matter.
     */
    std::array<float, Inputs> input{[this]() {
        if constexpr (Inputs == 1) {
            return pick_random_geopos(city_index)[1];
        } else {
            return pick_random_geopos(city_index);
        }
    }()};
};
/**
 *
 * ### Neurons
 *
 * Now we can get to something more directly applicable to our problem domain:
 * our neuron type. It's pretty simple as it's just a set of weights together
 * with a bias value.
 *
 */
#include "array.hpp"
template<std::size_t Weights>
struct neuron {
    /**
     * There is always a single bias value. We could keep it and the weights in
     * a single array, but for clarity let's split it out. In the video the bias
     * is generally started with a zero value in the worked through examples,
     * but setting the bias to a random weight also seems to work quite well,
     * and presumably can help.
     */
    float bias = random_weight();
    /**
     * The number of weights depends on the number of inputs, and we'll be using
     * both one and two. We also need to initialise the weights with random
     * numbers. The `random_weight` function returns a `float` between plus and
     * minus one{{footnote}}.
     *
     * {{footnote
     * To keep the implementation tidy, we define a function `array_of` in
     * [_array.hpp_](./array.hpp) which will give us an array built from
     * multiple copies of the input value, or by applying a lambda.
     * }}
     */
    std::array<float, Weights> weights =
            array_of<Weights>([]() { return random_weight(); });
};
/**
 *
 * A layer in the model is simply an array of these neurons.
 *
 */
template<std::size_t Weights, std::size_t Cities>
using layer_type = std::array<neuron<Weights>, Cities>;
/**
 *
 * {{footnote
 * We're going to be looping over arrays a lot, so it's useful to
 * have a higher order function that facilities this. The file
 * [_functional.hpp_](./functional.hpp) contains `by_index` which allows us to
 * easily iterate over one or more arrays by their indices.
 * }}
 *
 *
 * ### Model results
 *
 * A final utility type that we need will store the results of our model's
 * predictions{{footnote}}.
 *
 */
#include "functional.hpp"

template<std::size_t Inputs, std::size_t Cities>
struct result {
    /**
     * This type starts with a copy of the input example, and the probabilities
     * that are output from the model, one for each city. These fields need to
     * be set when a `result` is constructed.
     */
    ::example<Inputs, Cities> example;
    std::array<float, Cities> probabilities;
    /**
     * The answer that the model has output can be calculated from the
     * `probabilities` field -- we'll simply pick the one that has the highest
     * number. An {{include Acronym:/LLM.md}} might use the probability
     * distribution directly to randomly choose an output, but for what we're
     * doing here this is good and makes it easier to see how we'll get the
     * model to learn what we want it to.
     */
    std::size_t answer = [this]() {
        std::size_t answer{};
        by_index(1, Cities, [&](auto const index) {
            if (probabilities[index] > probabilities[answer]) {
                answer = index;
            }
        });
        return answer;
    }();
    /**
     * A final useful number is the _loss_. This is a measure of how wrong the
     * model is at the moment. The calculation here is something called the
     * "Cross Entropy Loss". We will track and display the loss value, but (as
     * we'll see later) it isn't actually used in the calculations for the back
     * propagation. It is a very useful number to help us to understand how far
     * off the model currently is though.
     */
    float loss = -std::log(probabilities[example.city_index]);
};
/**
 *
 * With all of our types set, we can now define our model and its parameters
 * together with some type aliases to make the rest of the code a bit clearer.
 *
 */
template<std::size_t Weights, std::size_t Cities>
struct model;

template<std::size_t Weights, std::size_t Cities>
struct parameters {
    static std::size_t constexpr weights = Weights;
    static std::size_t constexpr cities = Cities;


    using example_type = ::example<weights, cities>;
    using layer_type = ::layer_type<weights, cities>;
    using result_type = ::result<weights, cities>;

    using model = ::model<weights, cities>;
};
/**
 *
 * ## The prediction calculation
 *
 * Ultimately we need the model to make a prediction: Given an input location,
 * which city does it belong to?
 *
 *
 * ### Network weights
 *
 * The first step for this is to take the location and multiply the weights of
 * each neuron adding in the bias. This multiplication is known as the dot (or
 * inner) product. We'll define a multiply operator for this and, although the
 * standard library [does include a suitable
 * implementation](https://en.cppreference.com/w/cpp/algorithm/inner_product.html),
 * we'll implement it ourselves so it's clear what's going on.
 *
 */
#include <numeric>

template<std::size_t Weights>
auto operator*(
        ::neuron<Weights> const &neuron,
        std::array<float, Weights> const &input) {
    /**
     * Start a running total with the value in the `bias`, and then, for each
     * matched pair of floats in the `input` and the `neuron.weights` multiply
     * them and add them to the total. The returned value is a single `float`.
     */
    auto total = neuron.bias;
    by_index(Weights, [&](auto const index) {
        total += input[index] * neuron.weights[index];
    });
    return total;
}
/**
 *
 * This product needs to be calculated across the entire layer of neurons.
 *
 */
template<std::size_t Weights, std::size_t Cities>
auto operator*(
        layer_type<Weights, Cities> const &layer,
        std::array<float, Weights> const &input) {
    /**
     * Return an array of floats where we calculate the inner product for each
     * neuron against the input.
     */
    return array_of<Cities>(
            [&](auto const city) { return layer[city] * input; });
}
/**
 *
 * ### Softmax
 *
 * The output from this product calculation will give us a set of numbers, one
 * for each city, where larger values correspond to more certainty that the city
 * is the correct one. We could use these numbers directly, but it helps to
 * remap these arbitrarily large positive or negative numbers into a range
 * that's easier to deal with this. The function we'll use for this is called
 * "softmax" and is really two operations packed into one.
 *
 */
template<std::size_t Cities>
auto softmax(std::array<float, Cities> const &values) {
    /**
     * The first performs the remapping by raising e (Euler's Number) to the
     * output from each neuron. This amplifies large numbers by making them even
     * larger, but removes negative numbers by remapping larger negative numbers
     * to exponentially smaller positive numbers (nearer and nearer zero as the
     * negative value becomes larger). This gives us a set of numbers that are
     * all larger than zero.
     */
    std::array<float, Cities> output;
    float sum{};
    by_index(Cities, [&](auto const index) {
        /**
         * Because the outputs from the neurons can be arbitrarily large, and
         * the exponentiation can make these numbers even larger, it's useful to
         * cap them so that we don't get overflows. Overflows will result in
         * `inf` values and those in turn will lead to a breakdown of the model.
         * The cap of eighty gives us still very large outputs (which generally
         * the model won't reach), but small enough that they can still be
         * usefully summed so our algorithms will continue to work.
         */
        output[index] = std::exp(std::min(80.0f, values[index]));
        /**
         * We'll also need a sum of these values in order to normalise them in
         * the next step.
         */
        sum += output[index];
    });
    /**
     * Now that we have numbers that are all positive we can easily
     * normalise them into a range of zero to one, where the sum of all of
     * the numbers is also one. This is done by dividing each exponentiated
     * value by the sum of the exponentiated values.
     */
    for (auto &w : output) { w /= sum; }
    return output;
}
/**
 *
 * These compose into a single prediction calculation where we first put the
 * input through the neuron layer and run softmax on the result. This gives us
 * our predictions.
 *
 */
template<std::size_t Weights, std::size_t Cities>
auto calculate_result(
        layer_type<Weights, Cities> const &network,
        ::example<Weights, Cities> const &example) {
    /// Run the example input through the network
    auto const neuron_output = network * example.input;
    /// Calculate probabilities using softmax
    auto const sm = softmax(neuron_output);
    /**
     * We will return a `result` object, which will calculate the answer from
     * the probabilities and also a loss value (using the code we already talked
     * about above).
     */
    return result<Weights, Cities>{.example = example, .probabilities = sm};
}
/**
 *
 * ## Gradient descent
 *
 * Once we have an answer from the neural network, the next step is to alter it
 * in such a way that it will produce a better answer in the future. In the
 * video Welch Labs uses "Gradient Descent" for this. The basic idea is that the
 * model models a multi-dimensional surface with peaks, valleys and depressions.
 * The lowest points correspond to where the model will perform best, but it has
 * to get there from a random starting point. We can't model the entire surface,
 * that's just too computationally expensive, so instead we want to find a way
 * to get to the lowest point.
 *
 * When doing this in the real world (for example, walking down a mountain
 * looking for a way to the sea), a good heuristic is to simply walk down-hill
 * as much as you can. Eventually you'll most likely reach the sea (although it
 * may involve walking around some lakes to get there){{footnote}}.
 *
 * {{footnote
 * Of course there's a few places where you end up in a lake with no
 * exit, [but these are relatively
 * rare](https://en.wikipedia.org/wiki/Endorheic_basin). The conjection seems to
 * be that this is much more rare as the dimensionality of the model
 * increases.
 * }}
 *
 * The question then becomes: how to determine which direction is downhill from
 * where the model parameters currently sit? Luckily{{footnote}} calculus comes
 * to the rescue. If we can calculate the derivative of the surface at the
 * location of the model we will know which way is down. To calculate this
 * derivative it is enough to calculate derivatives of each step in our model
 * and multiply them together (this is called the chain rule, and is much
 * simpler than trying to calculate the derivative of our entire calculation).
 *
 * {{footnote
 * At least for some values of "luckily", depending on how you feel
 * about calculus.
 * }}
 *
 * The resulting derivatives (one per parameter) is then called the "Gradient
 * Descent Vector". By using the chain rule we can write code to calculate the
 * derivative of each step in our calculation (the inner product over the
 * weights and then the softmax calculation), We then compose them by
 * multiplying them to give us the final gradient descent vector.
 *
 * The derivative that we calculate this way points up the slope rather than
 * down the slope, but this simply means we subtract it from our model rather
 * than add it.
 *
 *
 * ### Layer derivative
 *
 * For the derivative of the neuron inner product it's useful to consider the
 * bias separately from the inner product of the weights.
 *
 */
template<std::size_t Weights, std::size_t Cities>
auto layer_derivative(::example<Weights, Cities> const &example) {
    /**
     * {{footnote
     * I feel uneasy about this. I wouldn't be surprised to learn that bias
     * derivative should be different, but it's unclear to me how it would
     * change. We are going to multiply this number by other terms (from the
     * softmax derivative) before we have our final result though, and from that
     * perspective one is the correct neutral value.
     * }}
     *
     * Because we're only dealing with lines, the derivative for the weights is
     * simply the input parameter. The derivative value for the bias used in the
     * videos appears to be one, but the video doesn't justify this. From
     * everything I remember about calculus I think technically this should be
     * zero, but because of the nature of how we'll apply the derivative, that
     * would mean that the bias would never change{{footnote}}.
     *
     * The derivative consists of a value for the bias and one for each weight.
     * We could put that into a separate data structure, but we already have one
     * that is the exact right shape and doesn't contain any baggage (in terms
     * of methods or other functionality that wouldn't be equally applicable to
     * the derivative), so let's just use the same data structure for it.
     */
    return neuron{.bias = 1.0f, .weights = example.input};
}
/**
 *
 * {{footnote
 * The video contains the full derivation, but the essential part is
 * that because we're using exponentiation and f(𝑥) = e<sup>𝑥</sup> is its own
 * derivative, we end up with a very simple result where the derivative is _more
 * or less_ equal to its input.
 * }}
 *
 *
 * ### Softmax derivative
 *
 * Calculating the derivative for the softmax function is more complex, but the
 * end result is quite simple{{footnote}}. Because the output of softmax is one
 * value per city, the derivative is also going to be one value per city.
 */
template<std::size_t Cities>
auto softmax_derivative(
        std::size_t const correct_result,
        std::array<float, Cities> const &softmax) {
    /**
     * For each city, the derivative is is the softmax output for that city minus
     * zero for every case except the actual answer, where we subtract one.
     */
    std::array<float, Cities> output = softmax;
    output[correct_result] -= 1.0f;
    /**
     * This leads to output where any probability for an incorrect city is a
     * positive value between zero and one, and the output for the correct city
     * is an output between minus one and zero. For example, if the model is
     * 100% sure that the answer is Madrid, but the real answer is Berlin, then
     * Madrid will have a value of one and Berlin will have a value of minus
     * one. All other cities will have a value of zero.
     *
     * In the case where the model is 100% sure, and it gets the right answer,
     * then all of the outputs will be zero, representing that there is nothing
     * left for the model to learn.
     */
    return output;
}
/**
 *
 * ### Combining the derivatives
 *
 * The next step is to combine the two partial derivatives to give us the full
 * gradient descent. To do this we multiply the two together. We're going to
 * need a number of parameters equal to the number of parameters in each neuron
 * times the number of cities. We can start by copying the layer derivative for
 * each city and then adjust the values by the softmax derivative values.
 */
template<std::size_t Weights, std::size_t Cities>
auto operator*(
        neuron<Weights> const &layer_derivative,
        std::array<float, Cities> const &softmax_derivative) {
    auto output = array_of<Cities>(layer_derivative);
    /**
     * We now need to multiply each of these values by the softmax derivative
     * for each city. This is easily achieved by a couple of nested
     * loops.
     */
    by_index(Cities, [&](auto const city) {
        output[city].bias *= softmax_derivative[city];
        by_index(Weights, [&](auto const weight) {
            output[city].weights[weight] *= softmax_derivative[city];
        });
    });
    return output;
}
/**
 *
 * ## Applying the gradient
 *
 * In order to make use of the gradient we have to apply it to the model's
 * current network. The first step is to multiply the gradient by a small number
 * -- we don't want to move too far down the gradient as that means we may skip
 * past a change in direction, or in the worst case skip past the minimum. The
 * downside is that if this value is too small then the model will learn very
 * slowly.
 *
 * In any case, we're going to need to multiply each value in the gradient by a
 * scalar. This operation is easily done in-place.
 */
template<std::size_t Weights, std::size_t Cities>
void operator*=(std::array<neuron<Weights>, Cities> &layer, float alpha) {
    /// Again, a couple of nested loops allows us to describe this change.
    by_index(Cities, [&](auto const city) {
        layer[city].bias *= alpha;
        by_index(Weights, [&](auto const weight) {
            layer[city].weights[weight] *= alpha;
        });
    });
}
/**
 * The gradient that we've calculated points up the slope, not down the slope.
 * This means that we need to subtract the gradient from the models' network
 * rather than add it{{footnote}}.
 */
template<std::size_t Weights, std::size_t Cities>
void operator-=(
        layer_type<Weights, Cities> &layer,
        layer_type<Weights, Cities> const &gradient) {
    by_index(Cities, [&](auto const city) {
        layer[city].bias -= gradient[city].bias;
        by_index(Weights, [&](auto const weight) {
            layer[city].weights[weight] -= gradient[city].weights[weight];
        });
    });
}
/**
 *
 * {{footnote
 * The last three functions are extremely similar to each other, with the exact
 * same pair of nested loops, but important differences in how the values to be
 * assigned are calculated. What's the higher order function there that will
 * allow us to abstract out the looping construct from the operations carried
 * out?
 *
 * The loops aren't particularly safe either. What happens if we accidentally
 * use `Cities` instead of `Weights` in the inner loop? We'd run off the end of
 * the array, and that would be bad. The compiler isn't going to give you an
 * error or warning with the code as we have it, even with warning levels
 * cranked up.
 *
 * Does that make this code bad? Frankly, yes. It also makes exploring how to
 * fix this is an interesting exercise. As this is an article about neural
 * networks and C++ _per se_ I've gone for simplicity of exposition. This is not
 * code that I'd want in a production system.
 * }}
 *
 * The final thing we need to do before starting to assemble everything is to be
 * able to print our data structures out so we can see what's happening. We also
 * want some sort of tracking for how the model is performing.
 *
 * {{footnote
 * A full description of how they work will have to wait for another time.
 * }}
 *
 * As the model runs we'll want to be able to get a general idea of how the
 * cross entropy loss measurement changes (it should decrease as the model
 * learns; is right more often; and gets more sure of its answers), and also
 * some idea of how often the model is right. Exponential decay functions
 * provide a convenient way to do this, and will allow us to track short term
 * and long term changes in these values with appropriate smoothing{{footnote}}.
 *
 */
#include "tiny-classifier.print.hpp"
#include "edf.hpp"
/**
 *
 * ## The model
 *
 * We can now start to define our model. We've already defined nearly all of
 * the actual functionality that we need, now we just need to put it together.
 * We'll also incorporate our tracking which will hopefully give us some insight
 * into how the model is getting on.
 *
 */
template<std::size_t Weights, std::size_t Cities>
struct model {
    using parameters = ::parameters<Weights, Cities>;
    using example_type = parameters::example_type;
    using layer_type = parameters::layer_type;
    using result_type = parameters::result_type;

    /**
     * The most important things that it needs to contain are the network that
     * we're working on, and the alpha number that will control the learning
     * rate.
     */
    layer_type network{};
    float alpha = 0.0001f;
    /**
     * The most basic tracking will be to count how many times we've done a
     * backpropagation step. This will allow us to see how long the model has
     * been running for. We'll call this number the `generation`.
     */
    std::size_t generation{};
    /**
     * We'll use exponential decay functions with half lives of between five and
     * 200 generations to measure both the success rate (how often our model is
     * right), and the cross entropy loss values as the model learns. These will
     * give us nicely smoothed out values that will allow us to see the trends
     * in at a glance.
     */
    std::array<exponential_decay, 4> success_rate{{{5}, {20}, {50}, {200}}};
    std::array<exponential_decay, 4> losses{{{5}, {20}, {50}, {200}}};
    /**
     * We can now talk about what a training step looks like. Given an example
     * (a city with a `geopos` describing a location in that city) we want the
     * model:
     *
     * 1. to predict which city the `geopos` belongs to (also called the
     * "forward pass");
     * 2. to use the `result` to perform our backpropagation (also called the
     * "backward pass").
     *
     * It's nice to print out what's happening whilst the model evaluates, but
     * it would be far too much to do so every generation. When we do print
     * though, we're going to want to print out the result and the model that
     * led to that result, so we'll print before we do the backpropagation.
     *
     * This leads to the below definition of `perform_step`.
     */
    auto perform_step(example_type const &example, bool const do_print) {
        auto const result = predict(example);
        if (do_print) { std::cout << result << *this << std::endl; }
        backpropagate(result);
    }
    /**
     * The actual prediction is simple enough. We just need to call
     * `calculate_result` we've already defined.
     */
    auto predict(example_type const &example) {
        auto const result = calculate_result(network, example);
        /**
         * Once we have a result we then use it to adjust our success and loss
         * tracking exponential decay functions.
         */
        adjust(losses, result.loss);
        adjust(success_rate,
               result.example.city_index == result.answer ? 100.0f : 0.0f);
        /**
         * The use of zero and 100 for the adjustment here allows us to see an
         * accuracy between these two values. If the model stops being correct
         * then the value will drop towards zero at the half lives we've
         * defined, similarly being always right means the value approaches 100
         * at the same half life rate. We can roughly interpret these values as
         * the percentage right over shorter or longer terms.
         */
        return result;
    }
    /**
     * We've also already done most of the hard work for the backpropagation. We
     * only have to combine the parts.
     */
    auto backpropagate(result_type const &result) {
        /**
         * We multiply our two derivatives to get the gradient (which points up
         * the slope).
         */
        auto gradient =
                layer_derivative(result.example)
                * softmax_derivative(
                        result.example.city_index, result.probabilities);
        /// Use the `alpha` value to scale the gradient.
        gradient *= alpha;
        /// And then subtract the gradient from the current network.
        network -= gradient;
        /**
         * Because we've done a backpropagation we also increment our
         * `generation` count.
         */
        ++generation;
    }
};
/**
 *
 * ## Running the model
 *
 * We'll obviously need a main, and as we're going to be printing a lot of
 * floats we can set a precision to display to a value that isn't too high.
 */
int main() {
    std::cout << std::fixed << std::setprecision(4);
    /**
     * We'll start with the first worked example. The only example value for
     * alpha given in the video is the 0.0001 that we used as the default. But
     * in the video the learning seems to be much faster. The below value of
     * 0.06 is much closer to what the video shows results for, but not the same.
     */

    one_d::model one{
            .network =
                    {{{.bias = 0, .weights = {1}},
                      {.bias = 0, .weights = {0}},
                      {.bias = 0, .weights = {-1}}}},
            .alpha = 0.06};
    one_d::example_type const paris{1, {2.3376f}};
    one.perform_step(paris, true);
    one_d::example_type const madrid{0, {-3.7033}};
    one.perform_step(madrid, true);
    /**
     * Running these first two generations gives the following output:
     *
     * ```text
     * X  Paris -> Madrid at 74.9984%
     * Probabilities [0.7500 0.2461 0.0039] Loss 1.4021
     * Generation 0
     * Correctness: 0.0000/0.0000/0.0000/0.0000
     * Losses: 0.1815/0.0478/0.0193/0.0049
     * {1.0000 [0.8144]} {0.0000 [0.7655]} {-1.0000 [-0.5756]}
     *
     * X  Madrid -> Berlin at 93.1205%
     * Probabilities [0.0563 0.0125 0.9312] Loss 2.8763
     * Generation 1
     * Correctness: 0.0000/0.0000/0.0000/0.0000
     * Losses: 0.5303/0.1441/0.0586/0.0148
     * {0.9550 [0.7092]} {0.0452 [0.8712]} {-1.0002 [-0.5762]}
     * ```
     *
     * The conclusions of the model are essentially the same as what is in the
     * video, but the details of numbers are off in various ways. It's unclear
     * why this is, and I think the likely culprits are (in decreasing order of
     * likelihood):
     *
     * 1. I've made some mistake somewhere
     * 2. The alpha value I'm using is too different
     * 3. There's a transcription error in the video
     * 4. The video example is really doing something a bit different to what it
     * describes
     *
     * The discrepancy doesn't make any real difference to the rest of the
     * results though. We'll let the model run until the correctness level for
     * the 20 generation half life exceeds 99.9%, and we'll print out every 25
     * generations.
     */
    while (one.success_rate[1]() < 99.9f) {
        one_d::example_type const example;
        one.perform_step(example, one.generation % 25 == 0);
    }
    std::cout << one << "\n\n";
    /**
     * This model produces a final set of weights after around 200 to 300
     * generations, typically around the 250 mark. For example:
     *
     * ```text
     * Generation 248
     * Correctness: 100.0000/99.9011/95.0215/53.6116
     * Losses: 0.0698/0.0801/0.1203/0.1489
     * {0.0600 [-1.2131]} {1.3007 [0.3246]} {-1.3607 [0.8885]}
     * ```
     *
     * This process takes less than 10ms on my seven year old laptop.
     *
     * We can also initialise the weights randomly with the recommended alpha
     * setting. This time we'll also wait until the 200 generation half life for
     * correctness to exceed 99.9% (a much stricter result). Typical runs are
     * now in the tens of thousands of generations:
     */
    one_d::model random{};
    while (random.success_rate[3]() < 99.9f) {
        one_d::example_type const example;
        random.perform_step(example, random.generation % 1000 == 0);
    }
    std::cout << random << "\n\n";
    /**
     *
     * ```text
     * Generation 54329
     * Correctness: 100.0000/100.0000/99.9998/99.9000
     * Losses: 0.1175/0.1882/0.2356/0.2732
     * {-0.6857 [-0.7041]} {0.2449 [0.5838]} {-0.3018 [0.7914]}
     * ```
     *
     * But, if the random weights align well, it can be very short:
     *
     * ```text
     * Generation 2621
     * Correctness: 100.0000/100.0000/99.9998/99.9002
     * Losses: 0.3942/0.3712/0.3728/0.3906
     * {-0.5453 [-0.4493]} {0.3573 [0.0254]} {-0.5893 [0.2398]}
     * ```
     *
     * On my laptop this version normally runs in less than 50ms.
     *
     *
     * ### Running the two dimensional model
     *
     * Our code is only aware of the difference between the one and two
     * dimensional cases in a single place (where it picks the random input to
     * use for the prediction). We'll let this one learn a bit faster, but we'll
     * still wait for the 200 generation half life of correctness to
     * exceed 99.9%.
     */
    two_d::model two{.alpha = 0.01f};
    while (two.success_rate[3]() < 99.9f) {
        two_d::example_type const example;
        two.perform_step(example, two.generation % 5000 == 0);
    }
    std::cout << two;
    /**
     *
     * Typical runs are now in the order of about 100,000 generations and take
     * about 100ms on my laptop.
     *
     * ```text
     * Generation 106136
     * Correctness: 100.0000/100.0000/99.9998/99.9001
     * Losses: 0.0025/0.0062/0.0079/0.0244
     * {-0.1755 [1.3366 -19.7131]} {-18.9953 [1.3644 -7.6687]} {-0.8787
     * [-0.7718 22.1136]} {20.1967 [-0.0950 3.6748]}
     * ```
     */
}
/**
 * It's kind of interesting to play around with things like the alpha to see how
 * it affects the results, and the models run quickly enough that
 * experimentation is fast. Have fun playing with it!
 *
 * ----
 *
 * Thanks to Jeroen Vermeulen, Thomas Natter and Paweł Wolny for reading early
 * drafts.
 */
