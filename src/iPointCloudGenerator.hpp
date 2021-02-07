#pragma once

#ifdef _WINDOWS
#ifdef _WINDLL
#define EXPORT_PCGEN __declspec(dllexport)
#else
#define EXPORT_PCGEN __declspec(dllimport)
#endif
#else
#define EXPORT_PCGEN
#endif

#include <string>
#include <limits>
#include "iStereoCamera.hpp"


class PointCloudCallback {
public:
	struct Point3d {
		float x, y, z;

		// �R���X�g���N�^
		// @param _x, _y, _z �O�������W�l
		// @note �f�t�H���g�l�͖����ȓ_���W�l��\���lstd::numeric_limits<float>::quiet_NaN()�ł���.
		Point3d(const float _x = std::numeric_limits<float>::quiet_NaN(), 
				const float _y = std::numeric_limits<float>::quiet_NaN(),
				const float _z = std::numeric_limits<float>::quiet_NaN()) : x(_x), y(_y), z(_z) {}
	};

	/**
	  iPointCloudGenerator.get_pointcloud(this)�Ɠn�����Ƃ��ɌĂяo�����R�[���o�b�N�֐�.
	  @return ����
	  @param [in] image �_�Q�ɒ���t������e�N�X�`���摜�̍���[�A�h���X
	  @param [in] step �e�N�X�`���摜�o�b�t�@�̐��������̃o�C�g��
	  @param [in] width �e�N�X�`���摜�̉���
	  @param [in] height �e�N�X�`���摜�̏c��
	  @param [in] points �_�Q�̎O�������W�l(�e�N�X�`���摜��Z�X�L�����������ɕ���ł��܂�)
	  @param [in] n_valid �L���ȓ_�̐�
	  @note image(i, j)�ɑΉ�����O�����_��points[i + j * width]�Ɋi�[����Ă��܂�.���̓_���L���ȓ_���ۂ��͍��W�l��
	  std::numeric_limits<float>::quiet_NaN()���i�[����Ă��邩�ۂ��Ŕ��f���Ă�������.
	 */
	virtual void operator()(
		unsigned char *image, const size_t step,
		const int width, const int height, 
		std::vector<Point3d> &points, const int n_valid) = 0;
};



class iPointCloudGenerator {
public:
	/**
	 * �_�Q�������j�����܂�.
	 * @return �Ȃ�
	 */
	virtual void destroy() = 0;

	/**
	 * �_�Q������̏��������s���܂�.
	 * @return �Ȃ�.
	 * @param [in] cam �g�p����X�e���I�J�����̃C���X�^���X
	 */
	virtual void init(iStereoCamera *cam) = 0;

	/**
	 * �O�����_�̍��W�ϊ��s����Z�b�g���܂�.
	 * @return �����ɐ��������ꍇ��true, ���s�����ꍇ��false.
	 * @param [in] filename 4x4�̍��̕ϊ��s�񂪊i�[����Ă���t�@�C����(OpenCV��Mat�`��)
	 */
	virtual bool convert_coordinate(const std::string filename) = 0;

	/**
	 * �O�����_�̍��W�ϊ��s����Z�b�g���܂�.
	 * @return �����ɐ��������ꍇ��true, ���s�����ꍇ��false.
	 * @param [in] RT 4x4�̍��̕ϊ��s�񂪊i�[����Ă���vector(4x4=16�v�f)
	 */
	virtual bool convert_coordinate(std::vector<double> &RT) = 0;

	/**
	 * �_�Q�����ɕK�v�ȃp�����[�^���Z�b�g���܂�.
	 * @param [in] params �p�����[�^�^�ւ̃|�C���^
	 * @note params �͎�@�ɂ���ĈقȂ�܂�.�O���C�R�[�h�ňʑ��V�t�g�̏ꍇ��ParamPSFT.hpp���A
	 * �O�ʑ��ňʑ��V�t�g�̏ꍇ��ParamMPSFT.hpp���ASGBM�̏ꍇ��ParamSGBM.hpp���Q�Ƃ��Ă�������.
	 */
	virtual bool setparams(void *params) = 0;

	/**
	 * �_�Q�𐶐����邽�߂ɕK�v�ȉ摜����
	 * @return �K�v�ȉ摜����
	 * @note 1��SGBM�̏ꍇ. �ʑ��V�t�g�̏ꍇ�͎�@�ɂ���ĈقȂ�
	 */
	virtual const size_t requiredframes() const { return 1; }


	/**
	 * �_�Q����������Z�b�g���܂�.(�����Ɏc���Ă���v���W�F�N�^�摜���폜���܂�.SGBM�ł͉������܂���.)
	 */
	virtual bool reset() { return true; }

	/**
	 * �����Ώۉ摜��_�Q������ɓn���܂�.
	 * @return ���������ꍇ��true, ���s�����ꍇ��false.
	 * @param [in] grayim �摜�o�b�t�@����[�A�h���X(�����摜�̂݉�)
	 * @param [in] step �摜�o�b�t�@�̐��������̃o�C�g��
	 * @param [in] cam �J�����ԍ�
	 * @param [in] idx �摜�ԍ�(�ʑ��V�t�g�̏ꍇ�K���K�v)
	 * @note ���E�A���摜(�摜���������J�����摜�A�E�����E�J�����摜�ƂȂ�悤�ɘA�����ꂽ�摜)��^����ꍇ�ɂ́Astep�͘A���摜��
	 * ���������̃o�C�g����^���A�J�����ԍ���2�Ƃ��Ă�������.
	 */
	virtual bool setpict(void *grayim, const size_t step, const int cam, const int idx = 0) = 0;

	/**
	 * �w�肳�ꂽ�t�@�C�����珈���Ώۉ摜��ǂݍ���ŁA�_�Q������ɓn���܂�.
	 * @return �ǂݍ���(���_�Q������ɃZ�b�g)�����������ꍇ��true, ���s�����ꍇ��false.
	 * @param [in] filename �摜�t�@�C����
	 * @param [in] cam �Z�b�g����J�����ԍ�(0: ���J����, 1: �E�J����, 2: ���J����(�A������͘A���摜�̂ݐ���ɓ��삷��)
	 * @param [in] idx �摜�ԍ�(�ʑ��V�t�g�̏ꍇ�K���K�v)
	 * @note ���E�A���摜(�摜���������J�����摜�A�E�����E�J�����摜�ƂȂ�悤�ɘA�����ꂽ�摜)��^����ꍇ�ɂ́A�J�����ԍ���2
	 * �Ƃ��Ă�������.
	 */
	virtual bool loadpict(std::string filename, const int cam, const int idx = 0) = 0;

	/**
	 * setpict or loadpict�ɂ���ē_�Q������ɓn���ꂽ�摜���A���N�e�B�t�@�C���ĕԂ��܂�.
	 * @return ���������ꍇ��true, ���s�����ꍇ��false.
	 * @param top [out] �摜�擪�A�h���X���i�[����ϐ��ւ̃|�C���^(�O�ŉ�����Ȃ��ł�������)
	 * @param width [out] �摜�������i�[����ϐ��ւ̃|�C���^
	 * @param height [out] �摜�c�����i�[����ϐ��ւ̃|�C���^
	 * @param step [out] �摜�o�b�t�@�̐��������̃o�C�g��
	 * @param [in] cam �J�����ԍ�(0: ���J����, 1: �E�J����, 2: �A���摜)
	 * @param [in] idx �摜�ԍ�(�ʑ��V�t�g�̏ꍇ�̂ݕK�v.SGBM�ł͖�������܂�). 0: ���摜, 1: ���摜�ƂȂ�.
	 * @warning setpict() or loadpict()�Ăяo����Apreprocess()�Ăяo���O�ɌĂяo���Ă�������. 
	 */
	virtual bool getpict(unsigned char **top, int *width, int *height, size_t *step, 
		const int cam, const int idx = 0) = 0;

	/**
	 * �_�Q������ɒ~����ꂽ�摜s����v���W�F�N�^���W�摜���쐬���܂�.(SGBM�ł͉������܂���)
	 * @return ���������������ꍇ��true, ���s�����ꍇ��false.
	 * @warning this->requiredframes()����Ԃ���閇����setpict() or loadpict()���Ăяo������ɕK��
	 * ���̊֐����Ăяo���Ă�������.
	 */
	virtual bool flushImageBuffer() { return true; }


	/**
	 * �_�Q�����̂��߂̑O�������s���܂�.
	 * @return �Ȃ�.
	 */
	virtual bool preprocess() = 0;

	/**
	 * �����}�b�v���쐬���܂�.
	 * @return �Ȃ�.
	 * @param [in] texture_cam �_�Q�ɒ���t����e�N�X�`�����ǂ���̃J��������̂��̂��g�p���邩?
	 * (0: ���J����(�]���ʂ�), 1: �E�J����)
	 */
	virtual void make_disparitymap(const int texture_cam_ = 0) = 0;

	/// �O�������W�l���v�Z�����@
	enum Method3D {
		QMatrix = 0,	///< Q�s��
		SVector,		///< �����x�N�g������
		PMatrix			///< P�s��
	};

	/**
	 * �_�Q�f�[�^���쐬���A�����o�b�t�@�ɕۑ����܂�.
	 * @param [in] method �O�������W�l���v�Z�����@(Method3D�Q��)
	 */
	virtual void generate_pointcloud(const Method3D method = Method3D::QMatrix) = 0;


	/**
	 * �쐬���ꂽ�_�Q�f�[�^���擾���܂�.(OnRevcPointCloud.operator()���Ăяo���܂�)
	 * @return �L���ȓ_�̐�
	 * @param [in] callback �R�[���o�b�N�֐��N���X�C���X�^���X�ւ̃|�C���^
	 */
	virtual int get_pointcloud(PointCloudCallback* callback) = 0;
};



/**
 * �_�Q�������[�h
 */
enum PcGenMode {
	PCGEN_SGBM = 0,	///< SGBM
	PCGEN_GRAYPS4,	///< �ʑ��V�t�g: Gray + 4step PS
	PCGEN_MULTI,	///< �ʑ��V�t�g: �}���`
};

/**
 * �_�Q��������쐬���܂�.
 * @return �_�Q������C���X�^���X�ւ̃|�C���^
 * @param [in] mode �_�Q������@(PcGenMode�̉��ꂩ)
 */
extern "C" EXPORT_PCGEN iPointCloudGenerator* CreatePointCloudGenerator(const int mode = 0);
typedef iPointCloudGenerator *(*pCreatePointCloudGenerator)(const int mode);
