// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*****************************************************************************
 *   Copyright (C) 2008-2009 by Andreas Lauser                               *
 *   Copyright (C) 2008-2009 by Melanie Darcis                               *
 *   Copyright (C) 2008-2009 by Klaus Mosthaf                                *
 *   Copyright (C) 2008 by Bernd Flemisch                                    *
 *   Institute for Modelling Hydraulic and Environmental Systems             *
 *   University of Stuttgart, Germany                                        *
 *   email: <givenname>.<name>@iws.uni-stuttgart.de                          *
 *                                                                           *
 *   This program is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation, either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *****************************************************************************/
/*!
 * \file
 *
 * \brief Adaption of the BOX scheme to the non-isothermal two-phase, two-component flow model.
 */
#ifndef DUMUX_IMMISCIBLE_NI_MODEL_HH
#define DUMUX_IMMISCIBLE_NI_MODEL_HH

#include "immiscibleniproperties.hh"

#include <dumux/boxmodels/immiscible/immisciblemodel.hh>

#include <string>

namespace Dumux {
/*!
 * \ingroup ImmiscibleNIModel
 * \brief A non-isothermal compositional primary variable switching model.
 *
 * This model implements a non-isothermal compositional flow of partly miscible fluids
 * \f$\alpha \in \{ 1, \dots, M \}\f$. Thus each component \f$\kappa \{ 1, \dots, N \}\f$ can be present in
 * each phase.
 * Using the standard multiphase Darcy approach a mass balance equation is
 * solved:
 * \f{eqnarray*}
    && \phi \frac{\partial (\sum_\alpha \varrho_\alpha X_\alpha^\kappa S_\alpha )}{\partial t}
    - \sum_\alpha \text{div} \left\{ \varrho_\alpha X_\alpha^\kappa
    \frac{k_{r\alpha}}{\mu_\alpha} \mbox{\bf K}
    (\text{grad}\, p_\alpha - \varrho_{\alpha} \mbox{\bf g}) \right\}\\
    &-& \sum_\alpha \text{div} \left\{{\bf D}_{\alpha, pm}^\kappa \varrho_{\alpha} \text{grad}\, X^\kappa_{\alpha} \right\}
    - \sum_\alpha q_\alpha^\kappa = 0 \qquad \kappa \in \{w, a\} \, ,
    \alpha \in \{w, n\}
 *     \f}
 * For the energy balance, local thermal equilibrium is assumed which results in one
 * energy conservation equation for the porous solid matrix and the fluids:
 * \f{eqnarray*}
    && \phi \frac{\partial \left( \sum_\alpha \varrho_\alpha u_\alpha S_\alpha \right)}{\partial t}
    + \left( 1 - \phi \right) \frac{\partial (\varrho_s c_s T)}{\partial t}
    - \sum_\alpha \text{div} \left\{ \varrho_\alpha h_\alpha
    \frac{k_{r\alpha}}{\mu_\alpha} \mathbf{K} \left( \text{grad}\,
     p_\alpha
    - \varrho_\alpha \mathbf{g} \right) \right\} \\
    &-& \text{div} \left( \lambda_{pm} \text{grad} \, T \right)
    - q^h = 0 \qquad \alpha \in \{w, n\}
\f}
 *
 * This is discretized using a fully-coupled vertex
 * centered finite volume (box) scheme as spatial and
 * the implicit Euler method as temporal discretization.
 *
 * By using constitutive relations for the capillary pressure \f$p_c =
 * p_n - p_w\f$ and relative permeability \f$k_{r\alpha}\f$ and taking
 * advantage of the fact that \f$S_w + S_n = 1\f$ and \f$X^\kappa_w + X^\kappa_n = 1\f$, the number of
 * unknowns can be reduced to two.
 * If both phases are present the primary variables are, like in the nonisothermal two-phase model, either \f$p_w\f$, \f$S_n\f$ and
 * temperature or \f$p_n\f$, \f$S_w\f$ and temperature. The formulation which ought to be used can be
 * specified by setting the <tt>Formulation</tt> property to either
 * <tt>ImmiscibleIndices::pWsN</tt> or <tt>ImmiscibleIndices::pNsW</tt>. By
 * default, the model uses \f$p_w\f$ and \f$S_n\f$.
 * In case that only one phase (nonwetting or wetting phase) is present the second primary
 * variable represents a mass fraction. The correct assignment of the second
 * primary variable is performed by a phase state dependent primary variable switch. The phase state is stored for all nodes of the system. The following cases can be distinguished:
 * <ul>
 *  <li>
 *    Both phases are present: The saturation is used (either\f$S_n\f$ or \f$S_w\f$, dependent on the chosen formulation).
 *  </li>
 *  <li>
 *    Only wetting phase is present: The mass fraction of air in the wetting phase \f$X^a_w\f$ is used.
 *  </li>
 *  <li>
 *    Only non-wetting phase is present: The mass fraction of water in the non-wetting phase, \f$X^w_n\f$, is used.
 *  </li>
 * </ul>
 */
template<class TypeTag>
class ImmiscibleNIModel : public ImmiscibleModel<TypeTag>
{
    typedef ImmiscibleModel<TypeTag> ParentType;
    typedef typename GET_PROP_TYPE(TypeTag, Indices) Indices;
    typedef typename GET_PROP_TYPE(TypeTag, Scalar) Scalar;

public:
    /*!
     * \brief Returns a string with the model's human-readable name
     */
    std::string name() const
    { return "immiscible_ni"; }

    /*!
     * \brief Given an primary variable index, return a human readable name.
     */
    std::string primaryVarName(int pvIdx) const
    { 
        if (pvIdx == Indices::temperatureIdx)
           return "temperature";
       
        return ParentType::primaryVarName(pvIdx);
    }

    /*!
     * \brief Given an equation index, return a human readable name.
     */
    std::string eqName(int eqIdx) const
    { 
        if (eqIdx == Indices::energyEqIdx)
           return "energy";
       
        return ParentType::eqName(eqIdx);
    }

    /*!
     * \brief Returns the relative weight of a primary variable for
     *        calculating relative errors.
     *
     * \param globalVertexIdx The global index of the vertex
     * \param pvIdx The index of the primary variable
     */
    Scalar primaryVarWeight(int globalVertexIdx, int pvIdx) const
    {
        if (pvIdx == Indices::temperatureIdx)
            return 1.0/300.0;

        return ParentType::primaryVarWeight(globalVertexIdx, pvIdx);
    }

    /*!
     * \brief Returns the relative weight of an equation
     *
     * \param globalVertexIdx The global index of the vertex
     * \param eqIdx The index of the primary variable
     */
    Scalar eqWeight(int globalVertexIdx, int eqIdx) const
    {
        if (eqIdx == Indices::energyEqIdx)
            // approximate heat capacity of 1kg of air
            return 1.0/1.0035e3;
        
        return ParentType::eqWeight(globalVertexIdx, eqIdx);
    }

protected:
    friend class BoxModel<TypeTag>;

    void registerVtkModules_()
    {
        ParentType::registerVtkModules_();
        this->vtkOutputModules_.push_back(new Dumux::BoxVtkEnergyModule<TypeTag>(this->problem_()));
    }
};

}

#include "immisciblenipropertydefaults.hh"

#endif