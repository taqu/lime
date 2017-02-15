#ifndef INC_LFRAMEWORK_STATEUTIL_H__
#define INC_LFRAMEWORK_STATEUTIL_H__
/**
@file StateUtil.h
@author t-sakai
@date 2016/11/10 create
*/
#include "lframework.h"

namespace lfw
{
#define STATEUTIL_DECLTYPE(CLASS_NAME, STATE_NUM, TYPENAME) \
    typedef lfw::StateUtil<CLASS_NAME, STATE_NUM> TYPENAME; \
    friend class lfw::StateUtil<CLASS_NAME, STATE_NUM>; \
    template<lfw::s32 N> void init(); \
    template<lfw::s32 N> void proc(); \
    template<lfw::s32 N> void term()

#define STATEUTIL_DESCLSTATEFUNC(STATE_NAME) \
    template<> void init<STATE_NAME>(); \
    template<> void proc<STATE_NAME>(); \
    template<> void term<STATE_NAME>()

    /**
    �X�e�[�g�ɉ����ăN���X�̃��\�b�h���Ăяo��

    @param T ... ���\�b�h���Ăяo���Ώ�
    @param N ... �X�e�[�g��
    */
    template<class T, s32 N>
    class StateUtil
    {
    public:
        typedef StateUtil<T, N> this_type;

        /**
        �����o�֐��|�C���^�e�[�u���������N���X

        @param I ... ����������X�e�[�g
        */
        template<s32 I>
        class FuncInitializer : public FuncInitializer<I-1>
        {
        public:
            template<class T, class U>
            FuncInitializer(T* t,U* u)
                :FuncInitializer<I-1>(t,u)
            {
                T::initFuncs_[I] = &U::init<I>;
                T::procFuncs_[I] = &U::proc<I>;
                T::termFuncs_[I] = &U::term<I>;
            }
        };

        template<>
        class FuncInitializer<0>
        {
        public:
            template<class T, class U>
            FuncInitializer(T*,U*)
            {
                T::initFuncs_[0] = &U::init<0>;
                T::procFuncs_[0] = &U::proc<0>;
                T::termFuncs_[0] = &U::term<0>;
            }
        };

        template<s32 I>
        friend class FuncInitializer;

        /**
        @param ���\�b�h���Ăяo���Ώ�
        */
        StateUtil(T* parent);

        //
        ~StateUtil();

        /**
        @brief �X�e�[�g�擾
        @return ���݂̃X�e�[�g
        */
        s32 get() const;

        /**
        @brief �X�e�[�g�Z�b�g
        @param state ... ���̃X�e�[�g
        */
        void set(s32 state);

        /**
        @brief ������
        @param state ... �����X�e�[�g
        */
        void initialize(s32 state);

        /// �X�V
        void update();
    private:
        typedef void (T::*Func)();
        typedef FuncInitializer<N-1> FuncInitializerType;

        s32 state_; /// ���݂̃X�e�[�g
        T* parent_; /// ���\�b�h���Ăяo���Ώ�

        static Func initFuncs_[N]; /// �������֐��e�[�u��
        static Func procFuncs_[N]; /// �X�V�֐��e�[�u��
        static Func termFuncs_[N]; /// �I���֐��e�[�u��
    };

    template<class T, s32 N>
    typename StateUtil<T,N>::Func StateUtil<T,N>::initFuncs_[N];

    template<class T, s32 N>
    typename StateUtil<T,N>::Func StateUtil<T,N>::procFuncs_[N];

    template<class T, s32 N>
    typename StateUtil<T,N>::Func StateUtil<T,N>::termFuncs_[N];

    template<class T, s32 N>
    StateUtil<T,N>::StateUtil(T* parent)
        :state_(-1)
        ,parent_(parent)
    {
        LASSERT(NULL != parent_);
        static FuncInitializerType initFunc(this, parent_);
    }

    template<class T, s32 N>
    StateUtil<T,N>::~StateUtil()
    {
        (parent_->*termFuncs_[state_])();
    }

    template<class T, s32 N>
    void StateUtil<T,N>::initialize(s32 state)
    {
        LASSERT(0<=state && state<N);
        state_ = state;
        (parent_->*initFuncs_[state_])();
    }

    template<class T, s32 N>
    void StateUtil<T,N>::update()
    {
        LASSERT(0<=state_ && state_<N);
        (parent_->*procFuncs_[state_])();
    }

    template<class T, s32 N>
    s32 StateUtil<T,N>::get() const
    {
        return state_;
    }

    template<class T, s32 N>
    void StateUtil<T,N>::set(s32 state)
    {
        LASSERT(0<=state_ && state_<N);

        (parent_->*termFuncs_[state_])();
        state_ = state;
        for(;;){
            lcore::s32 prev = state_;
            (parent_->*initFuncs_[prev])();
            if(state_ == prev){
                break;
            }
        }
    }


    /**
    ���\�b�h���Ăяo��

    @param T ... ���\�b�h���Ăяo���Ώ�
    @param N ... �X�e�[�g��
    */
    template<class T, s32 N>
    class StateProc
    {
    public:
        typedef StateProc<T, N> this_type;
        typedef void (T::*Func)();

        /**
        @param ���\�b�h���Ăяo���Ώ�
        */
        StateProc(T* parent);

        //
        ~StateProc();

        static void setFunc(s32 state, Func func);

        /**
        @brief �X�e�[�g�擾
        @return ���݂̃X�e�[�g
        */
        s32 get() const;

        /**
        @brief �X�e�[�g�Z�b�g
        @param state ... ���̃X�e�[�g
        */
        void set(s32 state);

        /// �X�V
        void update();
    private:

        s32 state_; /// ���݂̃X�e�[�g
        T* parent_; /// ���\�b�h���Ăяo���Ώ�

        static Func procFuncs_[N]; /// �X�V�֐��e�[�u��
    };

    template<class T, s32 N>
    typename StateProc<T,N>::Func StateProc<T,N>::procFuncs_[N];

    template<class T, s32 N>
    StateProc<T,N>::StateProc(T* parent)
        :state_(-1)
        ,parent_(parent)
    {
        LASSERT(NULL != parent_);
    }

    template<class T, s32 N>
    StateProc<T,N>::~StateProc()
    {
    }

    template<class T, s32 N>
    void StateProc<T,N>::setFunc(s32 state, Func func)
    {
        LASSERT(0<=state && state<N);
        procFuncs_[state] = func;
    }

    template<class T, s32 N>
    void StateProc<T,N>::update()
    {
        LASSERT(0<=state_ && state_<N);
        (parent_->*procFuncs_[state_])();
    }

    template<class T, s32 N>
    s32 StateProc<T,N>::get() const
    {
        return state_;
    }

    template<class T, s32 N>
    void StateProc<T,N>::set(s32 state)
    {
        LASSERT(0<=state && state<N);

        state_ = state;
    }
}
#endif //INC_LFRAMEWORK_STATEUTIL_H__
