#ifndef OPENPOSE__HAND__W_HAND_RENDERER_HPP
#define OPENPOSE__HAND__W_HAND_RENDERER_HPP

#include <memory> // std::shared_ptr
#include "../../thread/worker.hpp"
#include "handRenderer.hpp"

namespace op
{
    namespace experimental
    {
        template<typename TDatums>
        class WHandRenderer : public Worker<TDatums>
        {
        public:
            explicit WHandRenderer(const std::shared_ptr<HandRenderer>& handRenderer);

            void initializationOnThread();

            void work(TDatums& tDatums);

        private:
            std::shared_ptr<HandRenderer> spHandRenderer;

            DELETE_COPY(WHandRenderer);
        };
    }
}





// Implementation
#include "../../utilities/errorAndLog.hpp"
#include "../../utilities/macros.hpp"
#include "../../utilities/pointerContainer.hpp"
#include "../../utilities/profiler.hpp"
namespace op
{
    namespace experimental
    {
        template<typename TDatums>
        WHandRenderer<TDatums>::WHandRenderer(const std::shared_ptr<HandRenderer>& handRenderer) :
            spHandRenderer{handRenderer}
        {
        }

        template<typename TDatums>
        void WHandRenderer<TDatums>::initializationOnThread()
        {
            spHandRenderer->initializationOnThread();
        }

        template<typename TDatums>
        void WHandRenderer<TDatums>::work(TDatums& tDatums)
        {
            try
            {
                if (checkNoNullNorEmpty(tDatums))
                {
                    // Debugging log
                    dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
                    // Profiling speed
                    const auto profilerKey = Profiler::timerInit(__LINE__, __FUNCTION__, __FILE__);
                    // Render people hands
                    for (auto& tDatum : *tDatums)
                        spHandRenderer->renderHands(tDatum.outputData, tDatum.handKeyPoints);
                    // Profiling speed
                    Profiler::timerEnd(profilerKey);
                    Profiler::printAveragedTimeMsOnIterationX(profilerKey, __LINE__, __FUNCTION__, __FILE__, 1000);
                    // Debugging log
                    dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
                }
            }
            catch (const std::exception& e)
            {
                this->stop();
                tDatums = nullptr;
                error(e.what(), __LINE__, __FUNCTION__, __FILE__);
            }
        }

        COMPILE_TEMPLATE_DATUM(WHandRenderer);
    }
}

#endif // OPENPOSE__HAND__W_HAND_RENDERER_HPP
