class ConstraintGenerator:

    @staticmethod
    def activeSboxConstraints(in_vars, out_vars, r, k):
        """
        Generate the constraints of the k-th S-box at round r.
        Note, we include the r, and k parameters to index the activity marker variable
        k47 + k48 + k49 + k50 - y1 >= 0
        k47 - y1 <= 0
        k48 - y1 <= 0
        k49 - y1 <= 0
        k50 - y1 <= 0
        4 k47 + 4 k48 + 4 k49 + 4 k50 - vInSbox4 - vInSbox5 - vInSbox6 - vInSbox7 >= 0
        4 vInSbox4 + 4 vInSbox5 + 4 vInSbox6 + 4 vInSbox7 - k47 - k48 - k49 - k50 >= 0
        """
        dummy_var = 'S_' + str(r) + '_' + str(k)
        constraints = list([])
        s = ' + '.join(in_vars) + ' - ' + dummy_var + ' >= 0'
        constraints.append(s)
        for v in in_vars:
            constraints.append(v + ' - ' + dummy_var + ' <= 0')

        in_size = len(in_vars)
        out_size = len(out_vars)

        if in_size != out_size:
            for v in out_vars:
                constraints.append(v + ' - ' + dummy_var + ' <= 0')

        if in_size == out_size:
            c = ' + '.join([str(out_size) + ' ' + in_vars[j] for j in range(0, in_size)])
            c = c + ' - '
            c = c + ' - '.join(out_vars) + ' >= 0'
            constraints.append(c)

            c = ' + '.join([str(in_size) + ' ' + out_vars[j] for j in range(0, out_size)])
            c = c + ' - '
            c = c + ' - '.join(in_vars) + ' >= 0'
            constraints.append(c)

        return constraints

    @staticmethod
    def genFromConstraintTemplate(vars_list, ineq_template):
        """
        Example:
        >>> ConstraintGenerator.genFromConstraintTemplate(['e_ij', 'e_ik', 'e_jk'],basic_eij_tmplt)
        ['0 e_ij - 1 e_ik + 1 e_jk >= 0', '1 e_ij + 1 e_ik - 1 e_jk >= 0', '-1 e_ij + 0 e_ik + 1 e_jk >= 0']
        """
        assert ineq_template != list([])
        assert (len(vars_list)) == (len(ineq_template[1]) - 1)

       
        constraints = list([])
        for T in ineq_template:
            s = str(T[0]) + ' ' + vars_list[0]
            for j in range(1, len(vars_list)):
                if T[j] >= 0:
                    s = s + ' + ' + str(T[j]) + ' ' + vars_list[j]
                elif T[j] < 0:
                    s = s + ' - ' + str(-T[j]) + ' ' + vars_list[j]

            s = s + ' >= '
            if T[-1] <= 0:
                s = s + str(-T[-1])
            elif T[-1] > 0:
                s = s + '- ' + str(T[-1])

            constraints.append(s)

        return constraints

    @staticmethod
    def genSurgicalCuttingOffs(in_vars, out_vars, impossiblePatterns):
        """
        Example:
            >>> ConstraintGenerator.genSurgicalCuttingOffs(['d0','d1'], ['d2'], {(0,1, 0), (1,1,0)})
            ['-1 d0 - d1 + d2 >= -1', 'd0 - d1 + d2 >= 0']
            >>>
        """
        assert (len(in_vars) + len(out_vars)) == len(random.choice(list(impossiblePatterns)))

        vars_list = in_vars + out_vars
        constraints = list([])
        for badp in impossiblePatterns:
            c = ''
            if badp[0] == 1:
                c = c + '-1 ' + vars_list[0]
            else:
                c = c + vars_list[0]

            for i in range(1, len(vars_list)):
                if badp[i] == 1:
                    c = c + ' - ' + vars_list[i]
                else:
                    c = c + ' + ' + vars_list[i]

            c = c + ' >= ' + str(1 - sum(badp))
            constraints.append(c)

        return constraints

    
    @staticmethod
    def genEqualityConstraints(inputVar_list,outputVar_list):
        assert len(inputVar_list)==len(outputVar_list)
        constraints = list([])
        for i in range(len(inputVar_list)):
            s=inputVar_list[i]+' - '+outputVar_list[i] +' = 0'
            constraints.append(s)
        return constraints

    @staticmethod
    def genConsConstraints(inputVar_list,value):
        assert len(inputVar_list)==len(value)
        constraints = list([])
        for i in range(len(inputVar_list)):
            s=inputVar_list[i]+' = ' + value[i]
            constraints.append(s)
        return constraints
        
        

   
