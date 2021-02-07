#pragma once

#include "iStereoCamera.hpp"
#include "iPointCloudGenerator.hpp"
#include <chrono>
#include <vector>


class YPCGenerator {
public:
	YPCGenerator()
		: stereo(0), settings(), pcgen(0), method3d(iPointCloudGenerator::Method3D::QMatrix) {
	}

	virtual ~YPCGenerator() {
		if (stereo) stereo->destroy();
		if (pcgen) pcgen->destroy();
	}

	/**
	  �_�Q��������쐬���܂�.
	  @return �쐬�ɐ��������ꍇ��true, ���s�����ꍇ��false
	  @param [in] pcgen_mode �_�Q�������[�h(iPointCloudGenerator.hpp��PcGenMode���Q�Ƃ̎�)
	 */
	virtual bool create_pcgen(const PcGenMode pcgen_mode) = 0;

	/**
	  �_�Q�����p�����[�^�̓ǂݍ��݂��s���A�_�Q������Ƀp�����[�^��n���܂�.
	  @return �ǂݍ��݂ɐ��������ꍇ��true, ���s�����ꍇ��false.
	  @param [in] filename �p�����[�^�t�@�C����
	 */
	virtual bool init(const char *filename) = 0;

	/**
	  �_�Q�����ɕK�v�ȃX�e���I�J�������쐬���܂�.
	  @return �쐬�ɐ��������ꍇ��true, ���s�����ꍇ��false.
	  @param [in] dirname �J�����p�����[�^�t�@�C�����i�[����Ă���f�B���N�g����
	 */
	virtual bool create_camera(const char *dirname) = 0;

	/**
	  �J������HMat����\�z���ꂽ���ۂ���Ԃ��܂�.���W�n�ϊ��ς݂�HMat����X�e���I�J�������\�z���Ă���\�������邽��
	 */
	const bool is_camera_from_hmat() const {
		return (this->camtype == CamParamType::HMat) ? true : false;
	}

	/**
	  �_�Q�̍��W�ϊ��s����t�@�C������ǂݍ��݂܂�.
	  @return �ǂݍ��݂ɐ��������ꍇ��true, ���s�����ꍇ��false.
	  @param [in] filename ���W�ϊ��s�񂪋L�q����Ă���t�@�C����
	 */
	bool convert_coordinate(const char *filename) {
		if (!pcgen) return false;
		return pcgen->convert_coordinate(filename);
	}

	/**
	  �_�Q������ɓn���t�@�C���̃t���p�X�̃��X�g���쐬���܂�.
	  @return �쐬���ꂽ���X�g
	  @param [in] dirname �摜���i�[����Ă���f�B���N�g����
	  @param [in] file_ptn �t�@�C���̊g���q(?)
	  @note ���E�����摜���烊�X�g���쐬����ۂɂ́A�摜����0, �E0, ��1, �E1, ... �̏��ɂȂ�悤���ׂĂ�������.
	  @note �A���摜�͉����тŘA������Ă���摜�̂ݗL���ł�
	 */
	virtual std::vector<std::string> create_filelist(const char *dirname, const char *file_ptn) = 0;

	/**
	 * �_�Q������Ɏc���Ă���f�[�^���N���A���܂�.�_�Q�����O�Ɉ��K���Ăяo���Ă�������.
	 */
	bool reset();

	/**
	  �t�@�C������摜��ǂݍ���œ_�Q������ɓn���܂�.
	  @return �����ɐ��������ꍇ��true, ���s�����ꍇ��false.
	  @param [in] filenames �t�@�C���̃t���p�X���i�[����Ă���vector
	 */
	bool load_images(std::vector<std::string> &filenames);

	/**
	  �o�b�t�@����摜��_�Q������ɓn���܂�.
	  @return �����ɐ��������ꍇ��true, ���s�����ꍇ��false.
	  @param [in] buffers �摜����[�A�h���X���i�[����Ă���vector
	 */
	bool set_images(std::vector<unsigned char*> &buffers);

	/**
	  �o�ߎ��Ԃ�\�����܂�.
	  @return �Ȃ�
	 */
	virtual void print_elapsed() {}

public:
	/**
	 * 3D�}�b�`���O���s���O�ɕK�v�ȑO�������s���܂�.
	 * @return ���������������ꍇ��true, ���s�����ꍇ��false.
	 */
	bool preprocess();

	/**
	  ���������߁A�_�Q�𐶐����܂�.
	  @return �����������������ۂ�
	  @param [in] texture_cam �_�Q�ɒ���t����e�N�X�`�����ǂ���̃J��������̉摜���g�p���邩(0: ��(�]���ʂ�), 1: �E)
	 */
	bool execute(const int texture_cam);
	
	/**
	   �_�Q��ۑ����܂�.
	   @return �쐬���ꂽ�_�̐�
	   @param [in] callback �_�Q������ɌĂяo�����R�[���o�b�N�֐�
	 */
	int save_pointcloud(PointCloudCallback *callback) {
		return pcgen->get_pointcloud(callback);
	}

	// ����execute��save_pointcloud���܂Ƃ߂čs���֐�
	// generate_pointcloud�Ăяo���̑O�ɕK��load_images or set_images�ɂĉ摜��pcgen��
	// �^���Apreprocess()���Ăяo���Ă����Ȃ���΂Ȃ�Ȃ�.

	/**
	  �摜�t�@�C������摜��ǂݍ���œ_�Q�������s���܂�.
	  @return �쐬���ꂽ�_�̐�
	  @param [in] texture_cam �_�Q�ɒ���t����e�N�X�`�����ǂ���̃J��������̉摜���g�p���邩(0: ��(�]���ʂ�), 1: �E)
	  @param [in] callback �_�Q������ɌĂяo�����R�[���o�b�N�֐�
	 */
	int generate_pointcloud(const int texture_cam, PointCloudCallback *callback) {
		if (!this->execute(texture_cam)) return false;
		return this->save_pointcloud(callback);
	}

protected:
	/**
	  ���}�b�v��̉摜������Ԃ��܂�
	  @return ���}�b�v��̉摜����
	 */
	const int get_image_cols(void) const { return settings.output_cols; }

	/**
	  ���}�b�v��̉摜�c����Ԃ��܂�
	  @return ���}�b�v��̉摜�c��
	 */
	const int get_image_rows(void) const { return settings.output_rows; }

	/**
	  �J�����摜�̉�����ݒ肵�܂�
	  @return �Ȃ�
	  @param [in] wd �J�����摜����
	 */
	void set_camera_cols(const int cols) {
		settings.input_cols = cols;

		// ���̏��A���N�e�B�t�@�C��̉摜�T�C�Y == ���͉摜�T�C�Y�Ƃ��Ă����^�p���Ă��Ȃ��̂�
		// �o�͉摜�T�C�Y�������l��ݒ肷��.���A�Ⴄ�l���ݒ�ł���̂ŖY��Ȃ��悤��...
		settings.output_cols = cols;
	}

	/**
	  �J�����摜�̏c����ݒ肵�܂�
	  @return �Ȃ�
	  @param [in] ht �J�����摜�c��
	 */
	void set_camera_rows(const int rows) {
		settings.input_rows = rows;

		// ���̏��A���N�e�B�t�@�C��̉摜�T�C�Y == ���͉摜�T�C�Y�Ƃ��Ă����^�p���Ă��Ȃ��̂�
		// �o�͉摜�T�C�Y�������l��ݒ肷��.���A�Ⴄ�l���ݒ�ł���̂ŖY��Ȃ��悤��...
		settings.output_rows = rows;
	}

protected:
	/// �J�����^�C�v
	CamParamType camtype;

	/// �X�e���I�J����
	iStereoCamera *stereo;

	/// �X�e���I�J�����쐬�̂��߂̐ݒ�
	StereoCameraSettings settings;

	/// �_�Q������
	iPointCloudGenerator *pcgen;

	/// �_�Q�v�Z���@
	iPointCloudGenerator::Method3D method3d;

	std::chrono::system_clock::duration elapsed_phsdecode;	///< �ʑ�����
	std::chrono::system_clock::duration elapsed_preprocess;	///< �O�����ɂ�����������(�ʑ��ڑ��`�F�b�N�����N�e�B�t�@�C)
	std::chrono::system_clock::duration elapsed_makedisp;	///< �����v�Z�ɂ�����������
	std::chrono::system_clock::duration elapsed_genpcloud;	///< �_�Q�v�Z�ɂ�����������

	void time_start() { time_beg = std::chrono::system_clock::now(); }
	std::chrono::system_clock::duration get_elapsed() { 
		std::chrono::system_clock::time_point time_end = std::chrono::system_clock::now();	
		return std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_beg);
	}
	std::chrono::system_clock::time_point time_beg;
};



// PLY �t�@�C���ɓ_�Q�f�[�^��ۑ�����callback
class PLYSaver : public PointCloudCallback {
	std::string filename;
	bool status;

public:
	PLYSaver(const std::string name) : filename(name) {}

	const bool is_ok() const { return status; }
	const std::string get_filename() const { return filename; }

	/**
	  get_pointcloud�ɓn�����Ƃ��ɌĂяo�����callback�֐�(PLY�ɕۑ����܂�. �����ȓ_�͏��O���܂�.)
	  @note �ۑ��ɐ����������ۂ���status�ɕۑ�����܂�.
	 */
	void operator()(unsigned char *image, const size_t step, const int width, const int height,
		std::vector<Point3d> &points, const int n_valid);
};


// Depth�f�[�^��ۑ�����callback
class DepthSaver : public PointCloudCallback {
	std::string filename;
	bool status;

public:
	DepthSaver(const std::string name) : filename(name) {}

	const bool is_ok() const { return status; }
	const std::string get_filename() const { return filename; }

	/**
	   get_pointcloud�ɓn�����Ƃ��ɌĂяo�����callback�֐�(Depth�f�[�^(�ŏ�-�ő�Ő��K��)��ۑ����܂�.)
	   @note �ۑ��ɐ����������ۂ���status�ɕۑ�����܂�.
	 */
	void operator()(unsigned char *image, const size_t step, const int width, const int height,
		std::vector<Point3d> &points, const int n_valid);
};	
