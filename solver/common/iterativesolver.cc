
template <class OperatorType, class DiscFuncType>
inline
void Dune::IterativeSolver<OperatorType, DiscFuncType>::solve()
{

    int i;

    if (verbosity_ != QUIET)
        std::cout << "--- LinearSolver ---\n";

    double error = std::numeric_limits<double>::max();

    double normOfOldCorrection = 0;

    // Loop until desired tolerance or maximum number of iterations is reached
    for (i=0; i<numIt && (error>tolerance_ || isnan(error)); i++) {

        // Backup of the current solution for the error computation later on
        DiscFuncType oldSolution = iterationStep->getSol();

        // Perform one iteration step
        iterationStep->iterate();

        // Compute error
        double oldNorm = errorNorm_->compute(oldSolution, iterationStep->level());
        oldSolution -= iterationStep->getSol();

        double normOfCorrection = errorNorm_->compute(oldSolution, iterationStep->level());

        error = normOfCorrection / oldNorm;

        double convRate = normOfCorrection / normOfOldCorrection;

        normOfOldCorrection = normOfCorrection;

        // Output
        if (verbosity_ != QUIET) {
            std::cout << "||u^{n+1} - u^n||: " << error << ",      "
                      << "convrate " << convRate << "\n";
        }
        
    }

    if (verbosity_ != QUIET) {
        std::cout << i << " iterations performed\n";
        std::cout << "--------------------\n";
    }

}
